#include "cppcliargs.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
    cppcliargs::Config config{
        .defaults = {
            {'n', 20},
            {'f', "output.txt"}
        },
        .long_names = {
            {'n', "line-count"},
            {'f', "filename"}
        },
        .required = {'n'},
        .help = {
            {'n', "Number of lines to process"},
            {'f', "Output filename"}
        }
    };
    
    cppcliargs::parser p(config, argc, argv);
    
    if (p.help_requested()) {
        return 0;
    }
    
    auto result = p();
    if (!result) {
        p.report_error(result);
        return 1;
    }
    
    const auto& values = result.value();
    
    int line_count = values.get<int>('n');
    std::string filename = values.get<std::string>('f');
    
    std::cout << "Configuration:\n";
    std::cout << "  Lines:     " << line_count << "\n";
    std::cout << "  Filename:  " << filename << "\n";
    
    return 0;
}
