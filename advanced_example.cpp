#include "cppcliargs.hpp"
#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
    cppcliargs::Config config{
        .defaults = {
            {'n', 5},
            {'f', "input.txt"},
            {'o', "output.txt"},
            {'v', false}
        },
        .long_names = {
            {'n', "repeat"},
            {'f', "input"},
            {'o', "output"},
            {'v', "verbose"}
        },
        .required = {'f'},
        .help = {
            {'n', "Number of times to repeat the operation"},
            {'f', "Input file to process"},
            {'o', "Output file for results"},
            {'v', "Enable verbose logging"}
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
    
    int repeat = values.get<int>('n');
    std::string input_file = values.get<std::string>('f');
    std::string output_file = values.get<std::string>('o');
    bool verbose = values.get<bool>('v');
    
    if (verbose) {
        std::cout << "Processing configuration:\n";
        std::cout << "  Input:  " << input_file << "\n";
        std::cout << "  Output: " << output_file << "\n";
        std::cout << "  Repeat: " << repeat << " times\n";
    }
    
    std::ifstream input(input_file);
    if (!input) {
        std::cerr << "Error: Cannot open input file: " << input_file << "\n";
        return 1;
    }
    
    std::ofstream output(output_file);
    if (!output) {
        std::cerr << "Error: Cannot create output file: " << output_file << "\n";
        return 1;
    }
    
    std::string line;
    while (std::getline(input, line)) {
        for (int i = 0; i < repeat; ++i) {
            output << line << "\n";
            if (verbose) {
                std::cout << "Wrote: " << line << " (iteration " << (i+1) << ")\n";
            }
        }
    }
    
    if (verbose) {
        std::cout << "Processing complete!\n";
    }
    
    return 0;
}
