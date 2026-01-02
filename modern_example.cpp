#include "cppcliargs.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
    cppcliargs::Config config{
        .defaults = {
            {'n', 10},
            {'v', false}
        },
        .long_names = {
            {'n', "count"},
            {'v', "verbose"}
        },
        .help = {
            {'n', "Number of iterations"},
            {'v', "Enable verbose output"}
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
    
    int count = result.value().get<int>('n');
    bool verbose = result.value().get<bool>('v');
    
    for (int i = 1; i <= count; ++i) {
        if (verbose) {
            std::cout << "Iteration " << i << " of " << count << "\n";
        } else {
            std::cout << i << "\n";
        }
    }
    
    return 0;
}
