#include "cppcliargs.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

// Simple test to verify new API works
int main() {
    using namespace cppcliargs;
    
    std::cout << "Testing new API...\n";
    
    // Test 1: Simple parsing
    {
        const char* argv[] = {"test", "-a", "10", "-b", "20"};
        parser p({{'a', 0}, {'b', 0}}, 5, argv);
        
        if (p.help_requested()) {
            assert(false && "Should not request help");
        }
        
        auto result = p();
        assert(result.has_value());
        assert(result.value().get<int>('a') == 10);
        assert(result.value().get<int>('b') == 20);
        std::cout << "✓ Simple parsing\n";
    }
    
    // Test 2: Help request
    {
        const char* argv[] = {"test", "-h"};
        
        // Capture cout
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        
        parser p({{'a', 0}}, 2, argv);
        
        std::cout.rdbuf(old);  // Restore
        
        assert(p.help_requested());
        assert(buffer.str().find("Usage:") != std::string::npos);
        std::cout << "✓ Help request\n";
    }
    
    // Test 3: Error handling
    {
        const char* argv[] = {"test", "-a", "invalid"};
        parser p({{'a', 0}}, 3, argv);
        
        if (p.help_requested()) {
            assert(false);
        }
        
        auto result = p();
        assert(!result.has_value());
        assert(result.error().error == ParseError::InvalidIntegerValue);
        std::cout << "✓ Error handling\n";
    }
    
    // Test 4: Config with required fields
    {
        const char* argv[] = {"test", "-n", "100", "-f", "test.txt"};
        Config config{
            .defaults = {{'n', 0}, {'f', ""}},
            .required = {'n', 'f'}
        };
        
        parser p(config, 5, argv);
        
        if (p.help_requested()) {
            assert(false);
        }
        
        auto result = p();
        assert(result.has_value());
        assert(result.value().get<int>('n') == 100);
        assert(result.value().get<std::string>('f') == "test.txt");
        std::cout << "✓ Config with required\n";
    }
    
    // Test 5: Missing required
    {
        const char* argv[] = {"test", "-n", "100"};
        Config config{
            .defaults = {{'n', 0}, {'f', ""}},
            .required = {'n', 'f'}
        };
        
        parser p(config, 3, argv);
        
        if (p.help_requested()) {
            assert(false);
        }
        
        auto result = p();
        assert(!result.has_value());
        assert(result.error().error == ParseError::MissingRequiredArgument);
        std::cout << "✓ Missing required\n";
    }
    
    std::cout << "\nAll tests passed!\n";
    return 0;
}
