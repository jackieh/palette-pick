#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <Magick++.h>

#include "lib/color.h"
#include "lib/orientation.h"
#include "lib/stripes_image.h"

#include "tools/tools_common.h"

namespace {

namespace bpo = boost::program_options;

class MkStripes : public Tool {
 public:
    static const size_t default_width = 100;
    static const size_t default_length_multiplier = 100;
    static constexpr const char *default_orientation = "vertical";

    MkStripes() :
        help_(false),
        verbose_(false),
        width_(std::nullopt),
        length_(std::nullopt),
        orientation_(std::nullopt),
        output_file_(std::nullopt),
        colors_(std::vector<std::string>()),
        options_string_(std::string()) { }

    // Parse command line input into private members of this MkStripes
    // object. Return 0 if successful, or return a nonzero int if a fatal
    // error is encountered.
    int parse_options(int argc, char **argv) {
        try {
            try_parse_options(argc, argv);
        } catch (bpo::too_many_positional_options_error &error) {
            std::cerr << "Error: more than one output file specified"
                << std::endl;
            return exit_more_information();
        } catch (bpo::multiple_occurrences &error) {
            // Option X cannot be specified more than once.
            std::cerr << "Error: " << error.what() << std::endl;
            return exit_more_information();
        } catch (bpo::unknown_option &error) {
            // Unrecognized option X.
            std::cerr << "Error: unrecognized option \""
                << error.get_option_name() << "\"" << std::endl;
            return exit_more_information();
        } catch (bpo::invalid_command_line_syntax &error) {
            std::cerr << "Error: failed to interpret an argument after the "
                << error.tokens() << " option; "
                << "if the color value has a '#' character then you must "
                << "either place the color value in quotes or prefix the '#' "
                << "character with a '\\' character" << std::endl;
            return exit_more_information();
        }
        return 0;
    }

    // Gather any stdin text, each line of which is always interpreted as a
    // color.
    void parse_stdin() {
        std::string line;
        while (std::getline(std::cin, line)) {
            colors_.push_back(line);
        }
    }

    // Evaluate the collected command line options and export an image.
    // Return 0 if successful, or return a nonzero int if a fatal error is
    // encountered.
    //
    // Check for help, check for output file, then create an image and set
    // stripe colors/width/length/orientation and export the image to the
    // output file.
    int run() {
        if (help_) {
            return exit_help();
        }
        if (!output_file_.has_value()) {
            std::cerr << "Error: No output file specified" << std::endl;
            return exit_more_information();
        }

        const std::string stripe_orientation_string(
            orientation_.value_or(default_orientation));
        const palette::Orientation stripe_orientation(
            stripe_orientation_string);
        if (!stripe_orientation.valid()) {
            std::cerr << "\"" << stripe_orientation_string
                << "\" is not a valid orientation; it must be either "
                << "\"vertical\" or \"horizontal\"" << std::endl;
            return exit_more_information();
        }

        // Validate colors.
        std::vector<Magick::Color> magick_colors;
        for (const auto &color_str : colors_) {
            try {
                magick_colors.emplace_back(color_str);
            } catch (Magick::Exception &error) {
                std::cerr << "Warning: color option \"" << color_str
                    << "\" could not be interpreted as a color; ignoring"
                    << std::endl;
            }
        }
        if (magick_colors.empty()) {
            std::cerr << "No valid colors have been gathered; "
                << "generating an image requires at least one color"
                << std::endl;
            return exit_more_information();
        }

        const int stripe_length = length_.value_or(
            magick_colors.size() * default_length_multiplier);
        const int stripe_width = width_.value_or(
            static_cast<int>(default_width));
        palette::StripesImage image(stripe_length, stripe_width,
                                    stripe_orientation);
        for (const auto &magick_color : magick_colors) {
            image.get_stripe_colors().get().emplace_back(magick_color);
        }

        // Export the image.
        std::stringstream verbose_stream;
        std::stringstream error_stream;
        const bool export_success =
            image.export_image(output_file_.value(),
                               verbose_stream, error_stream);
        if (!export_success) {
            std::cerr << "Error: " << error_stream.str();
            return exit_more_information();
        }
        if (verbose_) {
            std::cout << verbose_stream.str();
        }
        return 0;
    }

 private:
    // Return the name of the executable file.
    std::string exec_name() { return EXEC_NAME; }

    // Return description of options constructed by a call to parse_options.
    std::string options_string() { return options_string_; }

    // Return summary of the functionality of this tool.
    std::string usage_string() {
        std::stringstream usage_stream;
        usage_stream << "Usage: " << exec_name()
            << " [arguments] [output]" << std::endl
            << "Create image file with stripes of specified colors."
            << std::endl << std::endl;
        usage_stream << "If generating an image, "
            << "then specifying at least one color is required."
            << std::endl
            << "Width/length/orientation arguments are optional."
            << std::endl;
        return usage_stream.str();
    }

    // Return description of specific examples of using this tool.
    std::string examples_string() {
        std::stringstream examples_stream;
        examples_stream << "Examples: " << exec_name()
            << " -c red -c orange -c yellow -o vertical output.png"
            << std::endl;
        examples_stream << "      or: " << exec_name()
            << " -c cyan -c \"#ffffff\" -c \"#FF0000\" "
            << "-l 500 -w 500 output.gif" << std::endl;
        return examples_stream.str();
    }

    // Create names and a description for each command line option.
    void create_options(bpo::options_description &opt,
                        bpo::positional_options_description &pos_opt) {
        const char *help_chars = "Print this help message and exit";
        const char *verbose_chars = "Print extra information to stdout";

        std::stringstream width_stream;
        width_stream << "Specify width in pixels of each stripe "
            << "in generated image (default " << default_width << ")";
        std::string width_string = width_stream.str();
        const char *width_chars = width_string.c_str();
        const auto *width_semantic(bpo::value<int>());

        std::stringstream length_stream;
        length_stream << "Specify length in pixels of stripes "
            << "in generated image (default " << default_length_multiplier
            << " multiplied by number of colors)";
        std::string length_string = length_stream.str();
        const char *length_chars = length_string.c_str();
        const auto *length_semantic(bpo::value<int>());

        std::stringstream orientation_stream;
        orientation_stream << "Specify orientation of stripes "
            << "as either \"vertical\" or \"horizontal\" (default "
            << default_orientation << ")";
        std::string orientation_string = orientation_stream.str();
        const char *orientation_chars = orientation_string.c_str();
        const auto *orientation_semantic(bpo::value<std::string>());

        const char *color_chars =
            "Specify an additional stripe by its color";
        const auto *color_semantic(bpo::value<std::vector<std::string>>());

        const char *output_chars = "Specify the path of output image file";
        const auto *output_semantic(bpo::value<std::string>());

        // TODO: Either implement an --input/-I option or create
        //       another tool to read color values from a text file.
        //       Example: mkstripes -I ~/.vim/colors/solarized.vim \
        //                -O solarized.png
        // TODO: Create an --annotate/-a option to annotate stripes
        //       with their hex values.
        // TODO: Consider creating an --unique/-U option that lets the
        //       user opt in/out of removal of duplicate colors.
        opt.add_options()
            ("help,h", help_chars)
            ("verbose,v", verbose_chars)
            ("width,w", width_semantic, width_chars)
            ("length,l", length_semantic, length_chars)
            ("orientation,o", orientation_semantic, orientation_chars)
            ("color,c", color_semantic, color_chars)
            ("output,O", output_semantic, output_chars);
        pos_opt.add("output", 1);

        std::stringstream options_stream;
        options_stream << opt;
        options_string_ = options_stream.str();
    }

    // Set private fields of this MkStripes object from the command line
    // options.
    void set_options(bpo::variables_map var_map) {
        help_ |= !var_map["help"].empty();
        verbose_ |= !var_map["verbose"].empty();
        if (!var_map["width"].empty()) {
            width_ = std::optional<int>(var_map["width"].as<int>());
        }
        if (!var_map["length"].empty()) {
            length_ = std::optional<int>(var_map["length"].as<int>());
        }
        if (!var_map["orientation"].empty()) {
            orientation_ = std::optional<std::string>(
                var_map["orientation"].as<std::string>());
        }
        if (!var_map["output"].empty()) {
            output_file_ = std::optional<std::string>(
                var_map["output"].as<std::string>());
        }
        if (!var_map["color"].empty()) {
            auto color_opts =
                var_map["color"].as< std::vector<std::string> >();
            colors_.insert(
                colors_.end(), color_opts.begin(), color_opts.end());
        }
    }

    bool help_;
    bool verbose_;
    std::optional<int> width_;
    std::optional<int> length_;
    std::optional<std::string> orientation_;
    std::optional<std::string> output_file_;
    std::vector<std::string> colors_;
    std::string options_string_;
};
}  // namespace

int main(int argc, char **argv) {
    Magick::InitializeMagick(*argv);
    MkStripes mkstripes_state;
    mkstripes_state.parse_stdin();
    int parse_options_result = mkstripes_state.parse_options(argc, argv);
    return ((parse_options_result == 0)
            ? mkstripes_state.run() : parse_options_result);
}
