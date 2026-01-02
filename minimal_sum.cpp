#include "cppcliargs.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
    // Simple: just defaults and argc/argv
    const cppcliargs::parser p({{'a', 0}, {'b', 0}}, argc, argv);
    
    // Help printed automatically if -h is used
    if (p.help_requested()) return 0;
    
    // Parse
    const auto result = p();
    if (!result) {
        p.report_error(result);  // Auto-prints error + help
        return 1;
    }
    
    // Use values
    const int a = result.value().get<int>('a');
    const int b = result.value().get<int>('b');
    std::cout << a + b << "\n";
}
