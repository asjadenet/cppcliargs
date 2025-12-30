#include "cppcliargs.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, const char* argv[]) {
    // Configure with designated initializers
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
    
    // Create parser with help support
    cppcliargs::parser p(config);
    
    // Check for help request using the built-in method
    if (p.has_help_request(argc, argv)) {
        std::cout << p.generate_help(argv[0]);
        return 0;
    }
    
    // Parse command line
    auto result = p(argc, argv);
    
    // Handle errors
    if (!result) {
        std::cerr << "❌ " << result.error().to_string() << "\n\n";
        std::cout << p.generate_help(argv[0]);
        return 1;
    }
    
    // Extract values with clean syntax
    const auto& values = result.value();
    bool verbose = values.get<bool>('v');
    int count = values.get<int>('n');
    std::string filename = values.get<std::string>('f');
    int threads = values.get<int>('t');
    
    // Validate values (custom business logic)
    if (count < 1) {
        std::cerr << "❌ Error: count must be at least 1\n";
        return 1;
    }
    
    if (count > 1000000) {
        std::cerr << "❌ Error: count too large (max: 1000000)\n";
        return 1;
    }
    
    if (threads < 1 || threads > 16) {
        std::cerr << "❌ Error: threads must be 1-16\n";
        return 1;
    }
    
    // Display parsed configuration
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║      Configuration Summary             ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    std::cout << "  📁 Input file:  " << filename << "\n";
    std::cout << "  🔢 Count:       " << count << "\n";
    std::cout << "  🧵 Threads:     " << threads << "\n";
    std::cout << "  📢 Verbose:     " << (verbose ? "yes" : "no") << "\n";
    std::cout << "\n";
    
    // Show all parsed arguments if verbose
    if (verbose) {
        std::cout << "═══════════════════════════════════════════\n";
        std::cout << "Detailed Argument Values:\n";
        std::cout << "═══════════════════════════════════════════\n";
        
        for (const auto& [key, value] : values) {
            std::cout << "  -" << key;
            
            if (config.long_names.contains(key)) {
                std::cout << " (--" << std::setw(12) << std::left 
                         << config.long_names.at(key) + ")";
            }
            
            std::cout << " = ";
            
            std::visit([](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, bool>) {
                    std::cout << (v ? "true" : "false");
                } else {
                    std::cout << v;
                }
            }, value);
            
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    
    // Simulate processing
    std::cout << "🚀 Processing started...\n";
    
    if (verbose) {
        std::cout << "   Using " << threads << " thread(s)\n";
        std::cout << "   Reading from: " << filename << "\n";
        std::cout << "   Processing " << count << " item(s)\n";
    }
    
    std::cout << "✅ Processing complete!\n";
    
    return 0;
}

/*
 * Example usage:
 * 
 * ./advanced_example -h
 * ./advanced_example -n 100 -f input.txt
 * ./advanced_example -n 100 -f input.txt -v
 * ./advanced_example -n 100 -f input.txt -v -t 8
 * 
 * Error examples:
 * ./advanced_example                          # Missing required args
 * ./advanced_example -n abc -f test.txt       # Invalid integer
 * ./advanced_example -n 100 -f test.txt -t 99 # Validation error
 */
