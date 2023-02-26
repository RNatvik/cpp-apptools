#ifndef APPTOOLS_COMMAND_LINE_PARSER
#define APPTOOLS_COMMAND_LINE_PARSER

#include <sstream>
#include <string>
#include <unordered_map>

/**
 * Struct containing information about the success or failure of parsing command line arguments.
 * The cause of potential errors will be reported on failure.
*/
struct CLInfo {
    bool success;
    std::string info;
};

/**
 * Command line argument parser.
 * 
 * The number of mandatory arguments can be set either in the constructor or by calling the method "set_num_args" after instantiation.
 * Flags can be declared in the constructor or by calling the method "add_flags".
 * Options (boolean flags) are declared as a single character.
 * Keyword arguments (flags with an argument) are declared as a single character followed by a ":" character.
 * Example: The flag string "abc:d" declares "-a", "-b", "-d" as options and "-c <value>" as keyword argument.
 * 
 * Parse the command line arguments by calling the method "parse".
 * The supplied CLInfo struct will provide information about the success or failure of the parse operation.
 * 
 * Retrieve boolean flags (options) by calling the method "get_opt".
 * Retrieve optional aruments (keyword arguments) by calling the method "get_kwarg".
 * Retrieve mandatory arguments by calling the method "get_arg".
 * 
 * TODO: Multi word string kwargs are broken. 
 * 
*/
class CLParser {
    public:
        /**
         * Constructor.
         * Defaults to 0 mandatory arguments and no options
        */
        CLParser() :
        _flags(), _kwargs()
        {
            this->_num_args = 0;
            this->_args = new std::string[1];
        }

        /**
         * Constructor.
         * Allocates supplied number of mandatory arguments and options.
         * Option string "abc:" creates option flags "-a", "-b" and keyword flag "-c <option_value>"
         * 
         * @param num_args: Number of mandatory arguments
         * @param flags: optional: formatted option string.
        */
        CLParser(int num_args, std::string flags="") :
        _flags(), _kwargs() 
        {
            this->_num_args = num_args;
            this->_args = new std::string[num_args];
            this->add_flags(flags);
        }

        ~CLParser() {
            delete[] this->_args;
        }

        /**
         * Sets the number of mandatory arguments
         * 
         * @param num_args: Number of mandatory arguments
        */
        void set_num_args(int num_args) {
            this->_num_args = num_args;
            if (this->_args != nullptr) {
                delete[] this->_args;
            }
            if (num_args < 1) {
                num_args = 1;
            }
            this->_args = new std::string[num_args];
        }

        /**
         * Adds flags to the parser
         * Option string "abc:" creates option flags "-a", "-b" and keyword flag "-c <option_value>"
         * 
         * @param options: formatted option string         
        */
        void add_flags(std::string options) {
            std::string flag;
            for (int i = 0; i < options.size(); i++) {
                flag = "-";
                flag += options[i];
                if (i < options.size() - 1 && options[i+1] == ':') {
                    this->_kwargs.emplace(flag, "");
                    i += 1;
                } else {
                    this->_flags.emplace(flag, false);
                }
            }
        }

        /**
         * Get option flag value
         * 
         * @param flag: option flag
         * @param invert: invert output (optional)
         * 
         * @return inverted or uninverted value of the supplied flag
        */
        bool get_opt(std::string flag, bool invert=false) {
            return this->_flags[flag] ^ invert;
        }

        /**
         * Get mandatory argument value
         * 
         * @tparam T: the type to interpret argument as.
         * 
         * @param index: the argument index number
         * 
         * @return value of the argument
        */
        template <typename T>
        T get_arg(int index) {
            return this->_convert_to<T>(this->_args[index]);
        }

        /**
         * Get optional argument value
         * 
         * @tparam T: the type to interpret argument as.
         * 
         * @param flag: the flag
         * @param default_value: The value to return if this option has not been supplied.
         * 
         * @return value of the argument
        */
        template <typename T>
        T get_kwarg(std::string flag, T default_value) {
            std::string str;
            T value;

            str = this->_kwargs[flag];
            if (str != "") {
                value = this->_convert_to<T>(str);
            } else {
                value = default_value;
            }
            return value;
        }

        /**
         * Get the application file path
         * 
         * @return the application file path
        */
        std::string get_file() {
            return this->_file;
        }

        /**
         * Parse the command line arguments
         * Supply argc and argv from main(int argc, char **argv) call
         * 
         * @param argc: argument count from main function
         * @param argv: argument vector from main function
         * @param info: CLInfo struct for parse details
        */
        void parse(int argc, char **argv, CLInfo &info) {
            int index, supplied_arguments;
            bool is_opt, is_kwarg;
            std::string str, kwarg;

            this->_file = argv[0];
            info.success = false;

            // Check number of supplied arguments
            supplied_arguments = 0;
            for (index = 1; index < argc; index++) {
                str = argv[index];
                is_opt = this->_flags.count(str) > 0;
                is_kwarg = this->_kwargs.count(str) > 0;
                if (is_opt || is_kwarg) {
                    break;  
                } else if (str.size() == 2 && str[0] == '-') {
                    break;             
                } else {
                    supplied_arguments += 1;
                }
            }

            if (supplied_arguments < this->_num_args) {
                // Too few arguments supplied
                info.info = "Error: Too few arguments supplied.\n";
                return;
            } else if (supplied_arguments > this->_num_args) {
                // Too many arguments supplied
                info.info = "Error: Too many arguments supplied.\n";
                return;
            }

            // Parse arguments
            for (index = 0; index < this->_num_args; index++) {
                str = argv[index+1];
                this->_args[index] = str;
            } 

            // Parse options and keyword arguments
            for (index = this->_num_args+1; index < argc; index++) {
                str = argv[index];
                if (this->_flags.count(str) > 0) {
                    this->_flags[str] = true;
                } else if (this->_kwargs.count(str) > 0) {
                    if (index < argc -1) {
                        index += 1;
                        kwarg = argv[index];
                        is_opt = this->_flags.count(kwarg) > 0;
                        is_kwarg = this->_kwargs.count(kwarg) > 0;
                        if (is_opt || is_kwarg) {
                            // Known option in keyword argument
                            info.info = "Error: Received option as argument to keyword \"" + str + "\".\n";
                            return;
                        } else if (kwarg.size() == 2 && kwarg[0] == '-') {
                            // Unknown option in keyword argument
                            info.info = "Error: Received unknown option as argument to keyword \"" + str + "\".\n";
                            return;
                        } else {
                            this->_kwargs[str] = kwarg; 
                        }
                    } else {
                        // No argument error
                        info.info = "Error: No argument given for keyword \"" + str + "\".\n";
                        return;
                    }
                } else {
                    info.info = "Error: Unknown option: \"" + str + "\".\n";
                    return;
                }
            }
            info.success = true;
            info.info = "Parse successful.";
        }


    private:
        std::string _file;
        std::string *_args;
        std::unordered_map<std::string, bool> _flags;
        std::unordered_map<std::string, std::string> _kwargs;
        int _num_args;

        template <typename T> 
        T _convert_to(const std::string &str) {
            std::istringstream ss(str);
            T num;
            ss >> num;
            return num;
        }

};

template <>
std::string CLParser::_convert_to<std::string>(const std::string &str) {
    return str;
}

#endif