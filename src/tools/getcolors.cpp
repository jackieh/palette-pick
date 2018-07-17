#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <Magick++.h>

#include <color_collection.h>

int main(int argc, char **argv) {
    boost::program_options::options_description opt_descr("Arguments");
    opt_descr.add_options()
        // TODO: Create an option to specify output format of colors.
        ("help,h", "Print this help message and exit")
        ("verbose,v", "Print extra information to stdout")
        ("number,n", boost::program_options::value<size_t>(),
         "Specify maximum number of colors to list from the image (default 16)")
        ("input,I", boost::program_options::value<std::string>(),
         "Input image file")
    ;

    boost::program_options::positional_options_description pos_opt_descr;
    pos_opt_descr.add("input", 1);

    std::stringstream usage_stream;
    usage_stream << "Usage: " << EXEC_NAME << " [arguments]" << std::endl;
    usage_stream << "List colors in hex format from specified file."
        << std::endl;

    std::stringstream example_stream;
    example_stream << "Examples: " << EXEC_NAME
        << " -I input.png" << std::endl;
    example_stream << "      or: " << EXEC_NAME
        << " --number 24 input.gif" << std::endl;

    boost::program_options::variables_map var_map;
    boost::program_options::command_line_parser parser(argc, argv);
    try {
        boost::program_options::store(
            parser.options(opt_descr).positional(pos_opt_descr).run(), var_map);
        boost::program_options::notify(var_map);
    } catch (boost::program_options::too_many_positional_options_error &error) {
        std::cerr << "Error: more than one input file specified" << std::endl;
        return 1;
    } catch (boost::program_options::unknown_option &error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    } catch (boost::program_options::invalid_command_line_syntax &error) {
        std::cerr << "Error: failed to interpret an argument after the "
            << error.tokens() << " option" << std::endl;
        return 1;
    }

    // Do the following in order:
    // 1. Check for help; exit if specified.
    // 2. Check for input; exit if not specified.
    // 3. Try to open the input file; exit if failed.
    // 4. Reduce colors via quantization if there are too many colors.
    // 5. Print out colors.

    if (var_map.count("help")) {
        std::cerr << usage_stream.str() << std::endl;
        std::cerr << example_stream.str() << std::endl;
        std::cerr << opt_descr << std::endl;
        return 1;
    }

    if (!var_map.count("input")) {
        std::cerr << "No input file specified" << std::endl;
        return 1;
    }
    std::string const input_file(var_map["input"].as<std::string>());

    size_t max_num_colors;
    if (var_map.count("number")) {
        max_num_colors = var_map["number"].as<size_t>();
    } else {
        max_num_colors = 16;
    }

    // Load image from input file.
    Magick::InitializeMagick(*argv);
    Magick::Image image;
    try {
        image.read(input_file);
    } catch (Magick::Exception &error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    size_t num_colors_original = image.totalColors();
    image.quantizeTreeDepth(max_num_colors);
    image.quantizeColors(max_num_colors);
    image.quantize();
    size_t num_colors_reduced = image.totalColors();
    if (var_map.count("verbose")) {
        std::cout << "Image " << input_file << " contains "
            << std::to_string(num_colors_original)
            << " colors; reduced to "
            << std::to_string(num_colors_reduced)
            << " colors" << std::endl;
    }

    palette::ColorCollection image_colors;
    for (size_t idx = 0; idx < image.colorMapSize(); ++idx) {
        Magick::Color color = image.colorMap(idx);
        image_colors.insert(color);
    }

    std::cout << image_colors.to_string("\n") << std::endl;
    return 0;
}
