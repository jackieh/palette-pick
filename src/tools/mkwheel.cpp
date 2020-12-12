#include <iostream>
#include <chrono>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <Magick++.h>

#include "tools_common.h"

namespace {

namespace bpo = boost::program_options;
namespace sch = std::chrono;

class MkWheel : public Tool {
 public:
    static const size_t default_height = 256;

    MkWheel() :
        help_(false),
        verbose_(false),
        height_(std::nullopt),
        type_(std::nullopt),
        color_(std::nullopt),
        output_file_(std::nullopt),
        options_string_(std::string()) { }

    // Parse command line input into private members of this MkWheel
    // object. Return 0 if successful, or return a nonzero int if a fatal
    // error is encountered.
    int parse_options(int argc, char **argv) {
        try {
            try_parse_options(argc, argv);
        } catch (bpo::too_many_positional_options_error &error) {
            std::cerr << "Error: More than one output file specified"
                << std::endl;
            return exit_more_information();
        } catch (bpo::multiple_occurrences &error) {
            // Option X cannot be specified more than once.
            std::cerr << "Error: " << error.what() << std::endl;
            return exit_more_information();
        } catch (bpo::unknown_option &error) {
            // Unrecognized option X.
            std::cerr << "Error: Unrecognized option \""
                << error.get_option_name() << "\"" << std::endl;
            return exit_more_information();
        } catch (bpo::invalid_command_line_syntax &error) {
            std::cerr << "Error: Failed to interpret an argument after the "
                << error.tokens() << " option";
            if (error.tokens().compare("--color") == 0) {
                std::cerr << "; if the color value has a '#' character then "
                    << "you must either place the color value in quotes or "
                    << "prefix the '#' character with a '\\' character";
            }
            std::cerr << std::endl;
            return exit_more_information();
        }
        return 0;
    }

    // Don't gather any stdin text.
    void parse_stdin() { return; }

    // Evaluate the collected command line options and export an image.
    // Return 0 if successful, or return a nonzero int if a fatal error is
    // encountered.
    int run() {
        sch::high_resolution_clock::time_point start_time =
            sch::high_resolution_clock::now();

        if (help_) {
            return exit_help();
        }
        if (!output_file_.has_value()) {
            std::cerr << "Error: No output file specified" << std::endl;
            return exit_more_information();
        }
        if (!color_.has_value()) {
            std::cerr << "Error: No color specified" << std::endl;
            return exit_more_information();
        }
        if (!type_.has_value()) {
            std::cerr << "Error: No color wheel type specified" << std::endl;
            return exit_more_information();
        }
        int image_height = height_.value_or(static_cast<int>(default_height));
        if (image_height <= 0) {
            std::cerr << "Error: Height option \"" << image_height
                << "\" specified; height must be a positive integer"
                << std::endl;
            return exit_more_information();
        }

        // TODO: Create more types of color wheels.
        int export_status = -1;
        if (type_.value().compare("hsl-hue") == 0) {
            export_status = run_hue(/* mirror */ false, image_height);
        } else if (type_.value().compare("hsl-hue-mirror") == 0) {
            export_status = run_hue(/* mirror */ true, image_height);
        } else {
            std::cerr << "Error: Unrecognized color wheel type \""
                << type_.value() << "\"" << std::endl;
            return exit_more_information();
        }

        if (verbose_ && (export_status == 0)) {
            sch::high_resolution_clock::time_point end_time =
                sch::high_resolution_clock::now();
            auto duration = sch::duration_cast<sch::microseconds>(
                end_time - start_time).count();
            std::cout << "Created " << output_file_.value() << " in "
                << duration << " microseconds" << std::endl;
        }

        return export_status;
    }

 private:
    // Return the name of the executable file.
    std::string exec_name() { return EXEC_NAME; }

    // Return description of options constructed by a call to parse_options.
    std::string options_string() { return options_string_; }

    // Return summary of the functionality of this tool.
    std::string usage_string() {
        std::stringstream usage_stream;
        usage_stream << "Usage: " << exec_name() << " [arguments] [output]"
            << std::endl << "Create color wheel image of specified properties."
            << std::endl << std::endl;
        usage_stream << "If generating an image, then "
            << "specifying exactly one color is required," << std::endl
            << "specifying exactly one image type is required, and "
            << "specifying exactly" << std::endl
            << "one output file is required." << std::endl;
        return usage_stream.str();
    }

    // Return description of specific examples of using this tool.
    std::string examples_string() {
        std::stringstream examples_stream;
        examples_stream << "Examples: " << exec_name()
            << " -c cyan -t hsl-hue-mirror -H 512 -O output.png" << std::endl;
        examples_stream << "      or: " << exec_name()
            << " -c \"#FF0000\" --type hsl-hue output.gif" << std::endl;
        return examples_stream.str();
    }

    // Create names and a description for each command line option.
    void create_options(bpo::options_description &opt,
                        bpo::positional_options_description &pos_opt) {
        const char *help_chars = "Print this help message and exit";
        const char *verbose_chars = "Print extra information to stdout";

        std::stringstream height_stream;
        height_stream << "Specify the height of the iamge to be generated "
            << "(default " << default_height << ")";
        std::string height_string = height_stream.str();
        const char *height_chars = height_string.c_str();
        const auto *height_semantic(bpo::value<int>());

        const char *type_chars = "Specify a type of color wheel to create "
            "(\"hsl-hue\" or \"hsl-hue-mirror\")";
        const auto *type_semantic(bpo::value<std::string>());

        const char *color_chars =
            "Specify a color on which to base the color wheel";
        const auto *color_semantic(bpo::value<std::string>());

        const char *output_chars = "Specify the path of output image file";
        const auto *output_semantic(bpo::value<std::string>());

        opt.add_options()
            ("help,h", help_chars)
            ("verbose,v", verbose_chars)
            ("height,H", height_semantic, height_chars)
            ("type,t", type_semantic, type_chars)
            ("color,c", color_semantic, color_chars)
            ("output,O", output_semantic, output_chars);
        pos_opt.add("output", 1);

        std::stringstream options_stream;
        options_stream << opt;
        options_string_ = options_stream.str();
    }

    // Set private fields of this MkWheel object from the command line
    // options.
    void set_options(bpo::variables_map var_map) {
        help_ |= !var_map["help"].empty();
        verbose_ |= !var_map["verbose"].empty();
        if (!var_map["height"].empty()) {
            height_ = std::optional<int>(var_map["height"].as<int>());
        }
        if (!var_map["type"].empty()) {
            type_ =  std::optional<std::string>(
                var_map["type"].as< std::string>());
        }
        if (!var_map["color"].empty()) {
            color_ =  std::optional<std::string>(
                var_map["color"].as< std::string>());
        }
        if (!var_map["output"].empty()) {
            output_file_ = std::optional<std::string>(
                var_map["output"].as<std::string>());
        }
    }

    // Generate an image that illustrates the HSL color space given a fixed
    // hue. Optionally include a "mirror" showing the color space of the
    // hue's complement. Return 0 if successful, or return a nonzero int if
    // a fatal error is encountered.
    int run_hue(const bool mirror, const int image_height) {
        const int image_width = (mirror ? image_height * 2 : image_height);
        double input_hue = 0;
        double input_saturation = 0;
        double input_lightness = 0;

        try {
            Magick::ColorHSL color_hsl(color_.value());
            input_hue = color_hsl.hue();
            input_saturation = color_hsl.saturation();
            input_lightness = color_hsl.lightness();
        } catch (Magick::Exception &error) {
            std::cerr << "Warning: color option \"" << color_.value()
                << "\" could not be interpreted as a color; ignoring"
                << std::endl;
            return exit_more_information();
        }

        if ((input_saturation == 0.0) || (input_lightness == 0.0)) {
            std::cerr.precision(2);
            std::cerr << "Warning: one or both of the input color's "
                << "saturation (" << std::fixed << input_saturation
                << ") or lightness (" << std::fixed << input_lightness
                << ") were found to be zero, making the input color "
                << "indistinguishable from a greyscale color; "
                << "guessing a red hue." << std::endl;
            input_hue = 0.0;
        }

        if (verbose_) {
            std::cout.precision(2);
            std::cout << "Constructing a color wheel based on hue value "
                << std::fixed << (360.0 * input_hue) << std::endl;
        }

        // Evaluate hue, saturation, and lightness of each pixel using
        // ImageMagick's fx expressions on the pixel's channels. Channel
        // types are not straightforwardly named, as RedChannel signifies
        // "channel 1," GreenChannel signifies "channel 2," etc. When the
        // image's color space is set (from the RGB default) to HSL, then
        // "channel 1" is the pixel's hue, "channel 2 is the pixel's
        // saturation, etc.
        Magick::Image wheel;
        wheel.size(Magick::Geometry(image_width, image_height));
        wheel.colorSpace(Magick::HSLColorspace);
        const Magick::ChannelType hue_channel = Magick::RedChannel;
        const Magick::ChannelType saturation_channel = Magick::GreenChannel;
        const Magick::ChannelType lightness_channel = Magick::BlueChannel;

        std::stringstream hue_fx_expr;
        hue_fx_expr.precision(20);
        hue_fx_expr << std::fixed << input_hue;
        wheel.fx(hue_fx_expr.str(), hue_channel);
        wheel.fx(mirror ? "(i-h)/h" : "i/w", saturation_channel);
        wheel.fx("(h-j)/h", lightness_channel);

        try {
            wheel.write(output_file_.value());
        } catch (Magick::Exception &error) {
            std::cerr << "ImageMagick exception: " << error.what() << std::endl;
            return exit_more_information();
        }

        return 0;
    }

    bool help_;
    bool verbose_;
    std::optional<int> height_;
    std::optional<std::string> type_;
    std::optional<std::string> color_;
    std::optional<std::string> output_file_;
    std::string options_string_;
};
}  // namespace

int main(int argc, char **argv) {
    Magick::InitializeMagick(*argv);
    MkWheel mkwheel_state;
    mkwheel_state.parse_stdin();
    int parse_options_result = mkwheel_state.parse_options(argc, argv);
    return ((parse_options_result == 0)
            ? mkwheel_state.run() : parse_options_result);
}
