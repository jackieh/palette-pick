TOP_DIR = .
BUILD_DIR = $(TOP_DIR)/build
SRC_DIR = $(TOP_DIR)/src
LIB_DIR = $(SRC_DIR)/lib
TOOLS_DIR = $(SRC_DIR)/tools

CXX = g++ -std=c++17 -g -Wall -Wextra -Weffc++ -Wno-comment
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
	rm -rf $(BUILD_DIR)/images/*
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
	mkdir -p $(BUILD_DIR)
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

TOOLS_COMMON_SRC = $(TOOLS_DIR)/tools_common.cpp
TOOLS_COMMON_OBJ = $(TOOLS_DIR)/tools_common.o

TOOLS_COMMON_BUILD = $(CXX) -I$(TOOLS_DIR) \
	-o $(TOOLS_COMMON_OBJ) \
	-c $(TOOLS_COMMON_SRC)

$(TOOLS_COMMON_OBJ): BUILD_FLAGS := -I$(TOOLS_DIR)

MKSTRIPES_SRC = $(TOOLS_DIR)/mkstripes.cpp
MKSTRIPES_OBJ = $(TOOLS_DIR)/mkstripes.o

MKSTRIPES_BUILD = $(CXX) \
	$(MAGICK_FLAGS) \
	-I$(LIB_DIR) \
	-I$(TOOLS_DIR) \
	-DEXEC_NAME=\"mkstripes\" \
	-o $(MKSTRIPES_OBJ) \
	-c $(MKSTRIPES_SRC)

MKSTRIPES_LINK = $(CXX) \
	$(MAGICK_FLAGS) \
	-o $(BUILD_DIR)/mkstripes \
	$(MKSTRIPES_OBJ) \
	-L$(BUILD_DIR) \
	-lboost_program_options \
	-lpalette \
	$(TOOLS_COMMON_OBJ)

# Target "mkstripes" to build the make-stripes tool.
.PHONY: mkstripes
mkstripes: $(LIB_OUT) $(TOOLS_COMMON_OBJ)
	$(MKSTRIPES_BUILD)
	$(MKSTRIPES_LINK)


MKWHEEL_SRC = $(TOOLS_DIR)/mkwheel.cpp
MKWHEEL_OBJ = $(TOOLS_DIR)/mkwheel.o

MKWHEEL_BUILD = $(CXX) \
	$(MAGICK_FLAGS) \
	-I$(LIB_DIR) \
	-I$(TOOLS_DIR) \
	-DEXEC_NAME=\"mkwheel\" \
	-o $(MKWHEEL_OBJ) \
	-c $(MKWHEEL_SRC)

MKWHEEL_LINK = $(CXX) \
	$(MAGICK_FLAGS) \
	-o $(BUILD_DIR)/mkwheel \
	$(MKWHEEL_OBJ) \
	-L$(BUILD_DIR) \
	-lboost_program_options \
	-lpalette \
	$(TOOLS_COMMON_OBJ)

# Target "mkwheel " to build the make-stripes tool.
.PHONY: mkwheel
mkwheel: $(LIB_OUT) $(TOOLS_COMMON_OBJ)
	$(MKWHEEL_BUILD)
	$(MKWHEEL_LINK)


GETCOLORS_SRC = $(TOOLS_DIR)/getcolors.cpp
GETCOLORS_OBJ = $(TOOLS_DIR)/getcolors.o

GETCOLORS_BUILD = $(CXX) \
	$(MAGICK_FLAGS) \
	-I $(LIB_DIR) \
	-I $(TOOLS_DIR) \
	-DEXEC_NAME=\"getcolors\" \
	-o $(GETCOLORS_OBJ) \
	-c $(GETCOLORS_SRC)

GETCOLORS_LINK = $(CXX) \
	$(MAGICK_FLAGS) \
	-o $(BUILD_DIR)/getcolors \
	$(GETCOLORS_OBJ) \
	-L$(BUILD_DIR) \
	-lboost_program_options \
	-lpalette \
	 $(TOOLS_COMMON_OBJ)

# Target "getcolors" to build the get-colors tool.
.PHONY: getcolors
getcolors: $(LIB_OUT) $(TOOLS_COMMON_OBJ)
	$(GETCOLORS_BUILD)
	$(GETCOLORS_LINK)


# Target "tools" to build all tools.
.PHONY: tools
tools: mkstripes mkwheel getcolors
