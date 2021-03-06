#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <Magick++.h>

#include "lib/color.h"
#include "lib/color_vector.h"
#include "lib/image.h"
#include "lib/image_get_sample_colors_mode.h"

#include "tools/tools_common.h"

namespace {

namespace bpo = boost::program_options;

class GetColors : public Tool {
 public:
    static const size_t default_quantize_tree_depth = 8;

    GetColors() :
        help_(false),
        verbose_(false),
        mode_(std::nullopt),
        max_num_colors_(std::nullopt),
        quantize_tree_depth_(std::nullopt),
        input_file_(std::nullopt),
        options_string_(std::string()) { }

    // Parse command line input into private members of this GetColors
    // object. Return 0 if successful, or return a nonzero int if a fatal
    // error is encountered.
    int parse_options(int argc, char **argv) {
        try {
            try_parse_options(argc, argv);
        } catch (bpo::too_many_positional_options_error &error) {
            std::cerr << "Error: more than one input file specified"
                << std::endl;
            return exit_more_information();
        } catch (bpo::unknown_option &error) {
            std::cerr << "Error: " << error.what() << std::endl;
            return exit_more_information();
        } catch (bpo::invalid_command_line_syntax &error) {
            std::cerr << "Error: failed to interpret an argument after the "
                << error.tokens() << " option" << std::endl;
            return exit_more_information();
        }
        return 0;
    }

    // Evaluate the collected command line options and read colors from an
    // image. Return 0 if successful, or return a nonzero int if a fatal
    // error is encountered.
    //
    // Check for help, check for input file, then load image, reduce colors
    // if the image originally has too many colors, then list the remaining
    // colors in the image.
    int run() {
        std::stringstream verbose_stream;
        if (help_) {
            return exit_help();
        }
        if (!mode_.has_value()) {
            std::cerr << "Error: No mode specified" << std::endl;
            return exit_more_information();
        }
        if (!input_file_.has_value()) {
            std::cerr << "Error: No input file specified" << std::endl;
            return exit_more_information();
        }
        if (!max_num_colors_.has_value()) {
            std::cerr << "Error: No number of colors specified" << std::endl;
            return exit_more_information();
        }
        if (*max_num_colors_ <= 0) {
            std::cerr << "Error: Number of colors must be a positive integer"
                << std::endl;
            return exit_more_information();
        }
        int quantize_tree_depth = quantize_tree_depth_.value_or(
            static_cast<int>(default_quantize_tree_depth));
        if (quantize_tree_depth < 0) {
            std::cerr << "Warning: depth option \"" << quantize_tree_depth
                << "\" is a negative integer; using default" << std::endl;
            quantize_tree_depth = static_cast<int>(default_quantize_tree_depth);
        }

        // Load image from input file.
        palette::Image image;
        try {
            image.get().read(input_file_.value());
        } catch (Magick::Exception &error) {
            std::cerr << error.what() << std::endl;
            return 1;
        }
        image.get().quantizeTreeDepth(quantize_tree_depth);

        palette::ImageGetSampleColorsMode mode(mode_.value());
        if (!mode.valid()) {
            std::cerr << "Unknown mode \"" << mode_.value() << "\""
                << std::endl;
            return 1;
        }

        bool get_sample_colors_success = false;
        std::vector<palette::Color> sample_colors = image.get_sample_colors(
            *max_num_colors_, mode, get_sample_colors_success);
        if (!get_sample_colors_success) {
            std::cerr << "Getting color subset failed" << std::endl;
            return 1;
        }
        std::sort(sample_colors.begin(), sample_colors.end());
        palette::ColorVector output_colors(std::move(sample_colors));
        std::cout << output_colors.to_string("\n") << std::endl;
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
        usage_stream << "Usage: " << exec_name() << " [arguments]" << std::endl;
        usage_stream << "List colors in hex format from specified file."
            << std::endl;
        return usage_stream.str();
    }

    // Return description of specific examples of using this tool.
    std::string examples_string() {
        std::stringstream examples_stream;
        examples_stream << "Examples: " << exec_name()
            << " -n 4 -I input.png" << std::endl;
        examples_stream << "      or: " << exec_name()
            << " --number 24 input.gif" << std::endl;
        return examples_stream.str();
    }

    // Create names and a description for each command line option.
    void create_options(bpo::options_description &opt,
                        bpo::positional_options_description &pos_opt) {
        const char *help_chars = "Print this help message and exit";
        const char *verbose_chars = "Print extra information to stdout";

        std::stringstream mode_stream;
        mode_stream << "Method for getting colors ("
            << "quantize" << ", "
            << "kmeans-random-spread" << ", "
            << "kmeans-static-spread" << ", "
            << "kmeans-hue-spread" << ", "
            << "kmeans-bright-hue-spread" << ", "
            << "kmeans-saturated-hue-spread" << ")";
        std::string mode_string = mode_stream.str();
        const char *mode_chars = mode_string.c_str();
        const auto *mode_semantic(bpo::value<std::string>());

        std::stringstream number_stream;
        number_stream << "Specify maximum number of colors to list "
            "from the image";
        std::string number_string = number_stream.str();
        const char *number_chars = number_string.c_str();
        const auto *number_semantic(bpo::value<int>());

        std::stringstream depth_stream;
        depth_stream << "Specify depth of tree used "
            << "in color quantization algorithm (default "
            << default_quantize_tree_depth << ")";
        std::string depth_string = depth_stream.str();
        const char *depth_chars = depth_string.c_str();
        const auto *depth_semantic(bpo::value<int>());

        std::stringstream input_stream;
        input_stream << "Input image file";
        std::string input_string = input_stream.str();
        const char *input_chars = input_string.c_str();
        const auto *input_semantic(bpo::value<std::string>());

        // TODO: Create an option to specify output format of colors.
        opt.add_options()
            ("help,h", help_chars)
            ("verbose,v", verbose_chars)
            ("mode,m", mode_semantic, mode_chars)
            ("number,n", number_semantic, number_chars)
            ("depth,d", depth_semantic, depth_chars)
            ("input,I", input_semantic, input_chars);

        pos_opt.add("input", 1);

        std::stringstream options_stream;
        options_stream << opt;
        options_string_ = options_stream.str();
    }

    // Set private fields of this GetColor object from the command line
    // options.
    void set_options(bpo::variables_map var_map) {
        help_ |= !var_map["help"].empty();
        verbose_ |= !var_map["verbose"].empty();
        if (!var_map["mode"].empty()) {
            mode_ = std::optional<std::string>(
                var_map["mode"].as<std::string>());
        }
        if (!var_map["number"].empty()) {
            max_num_colors_ = std::optional<int>(var_map["number"].as<int>());
        }
        if (!var_map["depth"].empty()) {
            quantize_tree_depth_ =
                std::optional<int>(var_map["depth"].as<int>());
        }
        if (!var_map["input"].empty()) {
            input_file_ = std::optional<std::string>(
                var_map["input"].as<std::string>());
        }
    }

    bool help_;
    bool verbose_;
    std::optional<std::string> mode_;
    std::optional<int> max_num_colors_;
    std::optional<int> quantize_tree_depth_;
    std::optional<std::string> input_file_;
    std::string options_string_;
};
}  // namespace

int main(int argc, char **argv) {
    Magick::InitializeMagick(*argv);
    GetColors getcolors_state;
    int parse_result = getcolors_state.parse_options(argc, argv);
    return (parse_result == 0) ? getcolors_state.run() : parse_result;
}
