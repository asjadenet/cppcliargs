#include "cppcliargs.hpp"
#include <iostream>

// This example demonstrates the preferred pattern of explicitly using
// cppcliargs:: namespace throughout the code

int main(int argc, const char* argv[])
{
    // Configure with designated initializers
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
    
    // Create parser instance
    cppcliargs::parser parser_inst(config);
    
    // Check for help request using the built-in method
    if (parser_inst.has_help_request(argc, argv)) {
        std::cout << parser_inst.generate_help(argv[0]);
        return 0;
    }
    
    // Parse command line arguments
    const auto parser_result = parser_inst(argc, argv);
    
    // Handle parsing errors
    if (!parser_result) {
        std::cerr << "Error: " << parser_result.error().to_string() << "\n\n";
        std::cout << parser_inst.generate_help(argv[0]);
        return 1;
    }
    
    // Access the parsed values
    const auto& parser_values = parser_result.value();
    
    // Extract specific values using clean get<T>() syntax
    bool show_help = parser_values.get<bool>('h');
    int line_count = parser_values.get<int>('n');
    std::string filename = parser_values.get<std::string>('f');
    
    // Display results
    std::cout << "Configuration:\n";
    std::cout << "  Help:      " << (show_help ? "yes" : "no") << "\n";
    std::cout << "  Lines:     " << line_count << "\n";
    std::cout << "  Filename:  " << filename << "\n";
    std::cout << "\n";
    
    // Iterate over all parsed arguments
    std::cout << "All arguments:\n";
    for (const auto& [key, value] : parser_values) {
        std::cout << "  variable '" << key << "' value: ";
        
        // Use std::visit to handle the variant
        std::visit([](const auto& v) {
            std::cout << v;
        }, value);
        
        std::cout << '\n';
    }
    
    return 0;
}
