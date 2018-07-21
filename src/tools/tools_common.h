#include <string>

#include <boost/program_options.hpp>

class Tool {
public:
    virtual ~Tool() { }
    virtual int parse_options(int argc, char **argv);
    virtual int run() = 0;

protected:
    int exit_help();
    int exit_more_information();
    void try_parse_options(int argc, char **argv);
    virtual void create_options(
        boost::program_options::options_description &opt,
        boost::program_options::positional_options_description &pos_opt) = 0;
    virtual void set_options(boost::program_options::variables_map var_map) = 0;

    virtual std::string help_option_name();
    virtual std::string exec_name() = 0;
    virtual std::string usage_string() = 0;
    virtual std::string examples_string() = 0;
    virtual std::string options_string() = 0;
};
