#include <iostream>
#include "clparser.hpp"

void print_help() {
    std::string str = "";
    str += "Usage: CLIExample <ip_address> [opts].\n";
    str += "    -h: Display this message.\n";
    str += "    -a: Some flag option.\n";
    str += "    -b: Some inverted flag option.\n";
    str += "    -p <port_number>: Port number.\n";
    std::cout << str << std::endl;

}

int main(int argc, char *argv[]) {
    // Configure parser
    CLInfo info;
    CLParser parser(1, "hap:b");
    parser.parse(argc, argv, info);

    // Check if successfull parse
    if (!info.success) {
        std::cout << info.info << std::endl;
        print_help();
        return -1;
    }

    // Check if help option is specified
    bool help = parser.get_opt("-h");
    if (help) {
        print_help();
        return 0;
    }

    // Extract values
    std::string file = parser.get_file();
    std::string ip = parser.get_arg<std::string>(0);
    bool some_flag = parser.get_opt("-a");
    bool some_flag_inverted = parser.get_opt("-b", true);
    int port = parser.get_kwarg<int>("-p", 5050);

    // Report values
    std::cout << "Filepath: " << file << std::endl;
    std::cout << "IP <arg 0>: " << ip << std::endl;
    std::cout << "Port [-p]: " << port << std::endl;
    std::cout << "Some flag [-a]: " << some_flag << std::endl;
    std::cout << "Some inverted flag [-b]: " << some_flag_inverted << std::endl;
    
    return 0;
}