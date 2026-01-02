#include "cppcliargs.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
    cppcliargs::Config config{
        .defaults = {
            {'v', false},
            {'n', 0},
            {'f', ""},
            {'t', 4}
        },
        .long_names = {
            {'v', "verbose"},
            {'n', "count"},
            {'f', "file"},
            {'t', "threads"}
        },
        .required = {'n', 'f'},
        .help = {
            {'v', "Enable verbose output"},
            {'n', "Number of iterations"},
            {'f', "Input filename"},
            {'t', "Thread count"}
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
    
    bool verbose = result.value().get<bool>('v');
    int count = result.value().get<int>('n');
    std::string file = result.value().get<std::string>('f');
    int threads = result.value().get<int>('t');
    
    if (verbose) {
        std::cout << "Processing " << file << " with " << count 
                  << " iterations using " << threads << " threads\n";
    } else {
        std::cout << "Processing: " << file << "\n";
    }
}
