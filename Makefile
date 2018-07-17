TOP_DIR = .
BUILD_DIR = $(TOP_DIR)/build
SRC_DIR = $(TOP_DIR)/src
LIB_DIR = $(SRC_DIR)/lib
TOOLS_DIR = $(SRC_DIR)/tools

CXX = g++
%.o: %.cpp
	$(CXX) $(BUILD_FLAGS) -o $@ -c $<

# Flags, libraries, and includes for external modules.
MAGICK_FLAGS = `Magick++-config --cppflags --cxxflags --ldflags --libs`

# Default target is to build library and all tools.
.PHONY: default
default: lib tools

RM_OBJ_CMD = rm -f $(TOP_DIR)/*.o; \
			 rm -f $(BUILD_DIR)/*.o; \
			 rm -f $(SRC_DIR)/*.o; \
			 rm -f $(LIB_DIR)/*.o; \
			 rm -f $(TOOLS_DIR)/*.o

# Target "cleanobj" to delete object files in source directories.
.PHONY: cleanobj
cleanobj:
	$(RM_OBJ_CMD)

# Target "clean" to delete files.
.PHONY: clean
clean:
	$(RM_OBJ_CMD)
	rm -f $(BUILD_DIR)/*


################################################################################
#  _     _ _
# | |   (_) |__  _ __ __ _ _ __ _   _
# | |   | | '_ \| '__/ _` | '__| | | |
# | |___| | |_) | | | (_| | |  | |_| |
# |_____|_|_.__/|_|  \__,_|_|   \__, |
#                               |___/
#

LIB_OBJ = \
		  $(LIB_DIR)/color_collection.o \
		  $(LIB_DIR)/stripes_image.o
LIB_SRC = $(LIB_OBJ:.o=.cpp)
$(LIB_DIR)/%.o: BUILD_FLAGS := -I$(LIB_DIR) $(MAGICK_FLAGS)
LIB_OUT = $(BUILD_DIR)/libpalette.a
$(LIB_OUT): $(LIB_OBJ)
	rm -f $@
	ar cq $@ $(LIB_OBJ)

# Target "lib" to build internal library.
.PHONY: lib
lib: $(LIB_OUT)


################################################################################
#   ____                                          _
#  / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |
# | |   / _ \| '_ ` _ \| '_ ` _ \ / _` | '_ \ / _` |
# | |__| (_) | | | | | | | | | | | (_| | | | | (_| |
#  \____\___/|_| |_| |_|_| |_| |_|\__,_|_| |_|\__,_|
#
#  _ _                _              _
# | (_)_ __   ___    | |_ ___   ___ | |___
# | | | '_ \ / _ \   | __/ _ \ / _ \| / __|
# | | | | | |  __/   | || (_) | (_) | \__ \
# |_|_|_| |_|\___|    \__\___/ \___/|_|___/
#

# Make rules for command line tools has more boilerplate because required flags
# and libraries may differ for each tool.
# TODO: Consider generalizing more things.

MKSTRIPES_SRC = $(TOOLS_DIR)/mkstripes.cpp
MKSTRIPES_OBJ = $(TOOLS_DIR)/mkstripes.o

MKSTRIPES_BUILD = $(CXX) \
				  -g \
				  -Wall -Wextra -Weffc++ \
				  -Wno-comment \
				  $(MAGICK_FLAGS) \
				  -I$(LIB_DIR) \
				  -DEXEC_NAME=\"mkstripes\" \
				  -o $(MKSTRIPES_OBJ) \
				  -c $(MKSTRIPES_SRC)

MKSTRIPES_LINK = $(CXX) \
				 -g \
				 $(MAGICK_FLAGS) \
				 -o $(BUILD_DIR)/mkstripes \
				 $(MKSTRIPES_OBJ) \
				 -L$(BUILD_DIR) \
				 -lboost_program_options \
				 -lpalette

# Target "mkstripes" to build the make-stripes tool.
.PHONY: mkstripes
mkstripes: $(LIB_OUT)
	$(MKSTRIPES_BUILD)
	$(MKSTRIPES_LINK)


GETCOLORS_SRC = $(TOOLS_DIR)/getcolors.cpp
GETCOLORS_OBJ = $(TOOLS_DIR)/getcolors.o

GETCOLORS_BUILD = $(CXX) \
				  -g \
				  -Wall -Wextra -Weffc++ \
				  -Wno-comment \
				  $(MAGICK_FLAGS) \
				  -I $(LIB_DIR) \
				  -DEXEC_NAME=\"getcolors\" \
				  -o $(GETCOLORS_OBJ) \
				  -c $(GETCOLORS_SRC)

GETCOLORS_LINK = $(CXX) \
				 -g $(MAGICK_FLAGS) \
				 -o $(BUILD_DIR)/getcolors \
				 $(GETCOLORS_OBJ) \
				 -L$(BUILD_DIR) \
				 -lboost_program_options \
				 -lpalette

# Target "getcolors" to build the get-colors tool.
.PHONY: getcolors
getcolors: $(LIB_OUT)
	$(GETCOLORS_BUILD)
	$(GETCOLORS_LINK)

# Target "tools" to build all tools.
.PHONY: tools
tools: mkstripes getcolors
