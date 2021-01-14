#include "tools/tools_common.h"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

// Bare minimum functionality. Ideally the derived class redefines this
// function to do more specific things with specific errors.
int Tool::parse_options(int argc, char **argv) {
    try {
        try_parse_options(argc, argv);
    } catch (boost::program_options::error &error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return exit_more_information();
    }
    return 0;
}

int Tool::exit_help() {
    std::cerr << usage_string() << std::endl << examples_string() << std::endl
        << options_string() << std::endl;
    return 1;
}

int Tool::exit_more_information() {
    std::cerr << "Run \"" << exec_name() << " " << help_option_name()
        << "\" for more information." << std::endl;
    return 1;
}

// Common boost::program_options boilerplate.
void Tool::try_parse_options(int argc, char **argv) {
    boost::program_options::options_description opt_descr("Arguments");
    boost::program_options::positional_options_description pos_opt_descr;
    create_options(opt_descr, pos_opt_descr);
    boost::program_options::variables_map var_map;
    boost::program_options::command_line_parser parser(argc, argv);
    auto &parser_opt = parser.options(opt_descr).positional(pos_opt_descr);
    boost::program_options::store(parser_opt.run(), var_map);
    boost::program_options::notify(var_map);
    set_options(var_map);
}

std::string Tool::help_option_name() {
    return "--help";
}
