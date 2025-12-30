#include "cppcliargs.hpp"
#include <iostream>

// This example demonstrates the designated initializer approach
// for clear, self-documenting configuration

int main(int argc, const char* argv[])
{
    // Designated initializers - clear and explicit!
    cppcliargs::Config config{
        .defaults = {
            {'v', false},
            {'n', 10},
            {'f', "output.txt"}
        },
        .long_names = {
            {'v', "verbose"},
            {'n', "count"},
            {'f', "file"}
        },
        .required = {'n'},
        .help = {
            {'v', "Enable verbose output"},
            {'n', "Number of items to process"},
            {'f', "Output file"}
        }
    };
    
    // Create parser
    cppcliargs::parser p(config);
    
    // Clean help check - no index-based loops!
    if (p.has_help_request(argc, argv)) {
        std::cout << p.generate_help(argv[0]);
        return 0;
    }
    
    // Parse arguments
    auto result = p(argc, argv);
    
    // Handle errors with detailed messages
    if (!result) {
        std::cerr << "Error: " << result.error().to_string() << "\n\n";
        std::cout << p.generate_help(argv[0]);
        return 1;
    }
    
    // Extract values with clean syntax
    const auto& values = result.value();
    
    bool verbose = values.get<bool>('v');
    int count = values.get<int>('n');
    std::string filename = values.get<std::string>('f');
    
    // Use the values
    std::cout << "Processing " << count << " items...\n";
    std::cout << "Output: " << filename << "\n";
    
    if (verbose) {
        std::cout << "\nVerbose mode enabled!\n";
        std::cout << "All arguments:\n";
        for (const auto& [key, value] : values) {
            std::cout << "  -" << key;
            if (config.long_names.contains(key)) {
                std::cout << " (--" << config.long_names.at(key) << ")";
            }
            std::cout << " = ";
            std::visit([](const auto& v) { std::cout << v; }, value);
            std::cout << "\n";
        }
    }
    
    return 0;
}

/*
 * Example usage:
 * 
 * Short arguments:
 *   ./modern_example -h
 *   ./modern_example -n 42
 *   ./modern_example -n 100 -f results.txt -v
 * 
 * Long arguments:
 *   ./modern_example --help
 *   ./modern_example --count 42
 *   ./modern_example --count=100 --file results.txt --verbose
 * 
 * Mixed:
 *   ./modern_example -n 100 --file=data.csv -v
 */
