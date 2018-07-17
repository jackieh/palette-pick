#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <Magick++.h>

#include <stripes_image.h>

int main(int argc, char **argv) {
    boost::program_options::options_description opt_descr("Arguments");
    opt_descr.add_options()
        // TODO: Either implement an --input/-I option or create another tool
        //       to read color values from a text file.
        //       Example: mkstripes -I ~/.vim/colors/solarized.vim \
        //                -O solarized.png
        // TODO: Create a way to input color values via stdin.
        //       Example: getcolors -n 8 ~/Pictures/wallpaper.png | mkstripes \
        //                -O bgcolors.png
        // TODO: Create an --annotate/-a option to annotate stripes with their
        //       hex values.
        // TODO: Consider creating an --unique/-U option that lets the user opt
        //       in/out of removal of duplicate colors.
        ("help,h", "Print this help message and exit")
        ("verbose,v", "Print extra information to stdout")
        ("width,w", boost::program_options::value<int>(),
         "Specify width in pixels of stripes in generated image (default 100)")
        ("length,l", boost::program_options::value<int>(),
         "Specify length in pixels of stripes in generated image (default 100 "
         "multiplied by number of colors)")
        ("orientation,o", boost::program_options::value<std::string>(),
         "Specify orientation of stripes as either \"vertical\" or "
         "\"horizontal\" (default vertical)")
        ("color,c", boost::program_options::value<std::vector<std::string>>(),
         "Specify an additional stripe by its color")
        ("output,O", boost::program_options::value<std::string>(),
         "Output image file")
    ;

    boost::program_options::positional_options_description pos_opt_descr;
    pos_opt_descr.add("output", 1);

    std::stringstream usage_stream;
    usage_stream << "Usage: " << EXEC_NAME << " [arguments] [output]"
        << std::endl;
    usage_stream << "Create image file with stripes of specified colors."
        << std::endl;
    usage_stream << std::endl;
    usage_stream << "If generating an image, then specifying at least one "
        << "color is required." << std::endl
        << "Width/length/orientation arguments are optional." << std::endl;

    std::stringstream example_stream;
    example_stream << "Examples: " << EXEC_NAME
        << " -c red -c orange -c yellow -o vertical output.png" << std::endl;
    example_stream << "      or: " << EXEC_NAME
        << " -c cyan -c \"#ffffff\" -c \"#FF0000\" -l 500 -w 500 output.gif"
        << std::endl;

    boost::program_options::variables_map var_map;
    boost::program_options::command_line_parser parser(argc, argv);
    try {
        boost::program_options::store(
            parser.options(opt_descr).positional(pos_opt_descr).run(), var_map);
        boost::program_options::notify(var_map);
    } catch (boost::program_options::too_many_positional_options_error &error) {
        std::cerr << "Error: more than one output file specified" << std::endl;
        return 1;
    } catch (boost::program_options::multiple_occurrences &error) {
        // Option X cannot be specified more than once.
        std::cerr << "Error: " << error.what() << std::endl;
    } catch (boost::program_options::unknown_option &error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    } catch (boost::program_options::invalid_command_line_syntax &error) {
        std::cerr << "Error: failed to interpret an argument after the "
            << error.tokens() << " option; if the color value has a '#' "
            << "character then you must either place the color value in "
            << "quotes or prefix the '#' character with a '\\' character"
            << std::endl;
        return 1;
    }

    // Do the following in order:
    // 1. Check for help; exit if specified.
    // 2. Check for output; exit if not specified.
    // 3. Check for colors; exit if no colors are successfully gathered.
    // 4. Gather other options.
    // 5. Try to export the image.

    if (var_map.count("help")) {
        std::cerr << usage_stream.str() << std::endl;
        std::cerr << example_stream.str() << std::endl;
        std::cerr << opt_descr << std::endl;
        return 1;
    }

    if (!var_map.count("output")) {
        std::cerr << "No output file specified" << std::endl;
        return 1;
    }
    std::string const output_file(var_map["output"].as<std::string>());

    Magick::InitializeMagick(*argv);
    palette::StripesImage image;

    // Gather colors from command line options.
    if (var_map.count("color")) {
        auto color_opts = var_map["color"].as< std::vector<std::string> >();
        for (auto const &color_str : color_opts) {
            try {
                Magick::Color color_obj(color_str);
                image.insert_color(color_obj);
            } catch (Magick::Exception &error) {
                std::cerr << "Warning: color option \"" << color_str
                    << "\" could not be interpreted as a color; ignoring"
                    << std::endl;
            }
        }
    }
    if (image.empty()) {
        std::cerr << "No colors have been gathered; colors are required in "
            << "order to generate an image" << std::endl;
        return 1;
    }

    // Gather other command line options.
    if (var_map.count("width")) {
        image.set_width(var_map["width"].as<int>());
    }
    if (var_map.count("length")) {
        image.set_length(var_map["length"].as<int>());
    }
    if (var_map.count("orientation")) {
        std::string orientation_str = var_map["orientation"].as<std::string>();
        image.set_orientation(orientation_str);
    }

    std::stringstream export_stream;
    std::stringstream error_stream;
    palette::StripesImage::ExportStatus export_status =
        image.export_image(output_file, export_stream, error_stream);
    switch (export_status) {
        case palette::StripesImage::SUCCESS:
            if (var_map.count("verbose")) {
                std::cout << export_stream.str();
            }
            return 0;
        case palette::StripesImage::FAIL:
            std::cerr << "Error: " << error_stream.str();
            return 1;
    }

    // If the control flow ends up here then the code is incorrect.
    assert(0);
}
