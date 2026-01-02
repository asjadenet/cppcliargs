#include "cppcliargs.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
    const cppcliargs::Config config{
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
    
    const cppcliargs::parser p(config, argc, argv);
    
    if (p.help_requested()) {
        return 0;
    }
    
    const auto result = p();
    if (!result) {
        p.report_error(result);
        return 1;
    }
    
    const bool verbose = result.value().get<bool>('v');
    const int count = result.value().get<int>('n');
    const auto file = result.value().get<std::string>('f');
    const int threads = result.value().get<int>('t');
    
    if (verbose) {
        std::cout << "Processing " << file << " with " << count 
                  << " iterations using " << threads << " threads\n";
    } else {
        std::cout << "Processing: " << file << "\n";
    }
}
