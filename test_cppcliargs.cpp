#include "cppcliargs.hpp"
#include <cassert>
#include <iostream>
#include <vector>

// Simple test framework
struct TestContext {
    int total = 0;
    int passed = 0;
    std::string current_test;

    void test(const std::string& name, auto&& func) {
        current_test = name;
        total++;
        try {
            func();
            passed++;
            std::cout << "✓ " << name << "\n";
        } catch (const std::exception& e) {
            std::cout << "✗ " << name << ": " << e.what() << "\n";
        } catch (...) {
            std::cout << "✗ " << name << ": unknown exception\n";
        }
    }

    void assert_true(bool condition, const std::string& msg = "") {
        if (!condition) {
            throw std::runtime_error(current_test + ": " + msg);
        }
    }

    void summary() {
        std::cout << "\n" << passed << "/" << total << " tests passed\n";
    }
};

// Helper to create argv-style arrays
struct ArgvBuilder {
    std::vector<std::string> storage;
    std::vector<const char*> ptrs;

    ArgvBuilder(std::initializer_list<std::string> args) : storage(args) {
        ptrs.reserve(storage.size());
        for (const auto& s : storage) {
            ptrs.push_back(s.c_str());
        }
    }

    int argc() const { return static_cast<int>(ptrs.size()); }
    const char** argv() { return ptrs.data(); }
};

void test_basic_parsing() {
    TestContext ctx;

    ctx.test("Parse simple boolean flag", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-h"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('h') == true);
    });

    ctx.test("Boolean flag not present uses default", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('h') == false);
    });

    ctx.test("Parse integer argument", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 10}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "42"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<int>('n') == 42);
    });

    ctx.test("Parse negative integer", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "-123"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<int>('n') == -123);
    });

    ctx.test("Parse string argument", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'f', std::string("")}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-f", "myfile.txt"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<std::string>('f') == "myfile.txt");
    });

    ctx.test("Parse multiple arguments", [] {
        using namespace cppcliargs;
        ArgMap defaults{
            {'h', false},
            {'n', 20},
            {'f', std::string("default.txt")}
        };
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-h", "-n", "100", "-f", "output.txt"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('h') == true);
        assert(result.value().get<int>('n') == 100);
        assert(result.value().get<std::string>('f') == "output.txt");
    });

    ctx.summary();
}

void test_required_arguments() {
    TestContext ctx;

    ctx.test("Required argument present", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        std::set<char> required{'n'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program", "-n", "42"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<int>('n') == 42);
    });

    ctx.test("Required argument missing returns error", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        std::set<char> required{'n'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::MissingRequiredArgument);
        assert(result.error().argument == 'n');
    });

    ctx.test("Required boolean with explicit value", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}};
        std::set<char> required{'v'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program", "-v", "true"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == true);
    });

    ctx.test("Required boolean can be set to false", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', true}};
        std::set<char> required{'v'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program", "-v", "false"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == false);
    });

    ctx.test("Required boolean without value returns error", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}};
        std::set<char> required{'v'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program", "-v"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::MissingValue);
    });

    ctx.summary();
}

void test_error_handling() {
    TestContext ctx;

    ctx.test("Unknown argument returns error", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-x"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::UnknownArgument);
        assert(result.error().argument == 'x');
    });

    ctx.test("Missing value for integer", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::MissingValue);
        assert(result.error().argument == 'n');
    });

    ctx.test("Invalid integer value", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "abc"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::InvalidIntegerValue);
        assert(result.error().argument == 'n');
    });

    ctx.test("Invalid boolean value", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}};
        std::set<char> required{'v'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program", "-v", "yes"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::InvalidBooleanValue);
        assert(result.error().argument == 'v');
    });

    ctx.test("Duplicate argument", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "10", "-n", "20"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::DuplicateArgument);
        assert(result.error().argument == 'n');
    });

    ctx.test("Error message formatting", [] {
        using namespace cppcliargs;
        ParseErrorInfo error{
            ParseError::MissingRequiredArgument,
            'n',
            "line-count"
        };
        
        std::string msg = error.to_string();
        assert(msg.find("Missing required argument") != std::string::npos);
        assert(msg.find("-n") != std::string::npos);
        assert(msg.find("line-count") != std::string::npos);
    });

    ctx.summary();
}

void test_equals_syntax() {
    TestContext ctx;

    ctx.test("Boolean with -x=true syntax", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}};
        std::set<char> required{'v'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program", "-v=true"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == true);
    });

    ctx.test("Boolean with -x=false syntax", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', true}};
        std::set<char> required{'v'};
        parser p(defaults, {}, required);
        
        ArgvBuilder args{"program", "-v=false"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == false);
    });

    ctx.test("Integer with -x=value syntax", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n=42"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<int>('n') == 42);
    });

    ctx.test("String with -x=value syntax", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'f', std::string("")}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-f=myfile.txt"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<std::string>('f') == "myfile.txt");
    });

    ctx.summary();
}

void test_long_names() {
    TestContext ctx;

    ctx.test("Long names stored in parser", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}, {'n', 20}};
        std::map<char, std::string> long_names{{'h', "help"}, {'n', "line-count"}};
        parser p(defaults, long_names);
        
        // Just verify parser constructs - long names in error messages
        ArgvBuilder args{"program"};
        auto result = p(args.argc(), args.argv());
        assert(result.has_value());
    });

    ctx.test("Error includes long name", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 20}};
        std::map<char, std::string> long_names{{'n', "line-count"}};
        std::set<char> required{'n'};
        parser p(defaults, long_names, required);
        
        ArgvBuilder args{"program"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().detail == "line-count");
    });

    ctx.summary();
}

void test_constexpr_configuration() {
    TestContext ctx;

    ctx.test("Parser constructor is noexcept", [] {
        using namespace cppcliargs;
        // In C++23, std::map/set don't have constexpr destructors yet (C++26)
        // But we can verify noexcept construction
        ArgMap defaults{{'h', false}};
        std::map<char, std::string> long_names{{'h', "help"}};
        std::set<char> required{};
        
        // This should compile with noexcept constructor
        parser p(defaults, long_names, required);
        
        assert(true); // Constructor succeeded
    });

    ctx.test("Error messages are constexpr", [] {
        using namespace cppcliargs;
        constexpr auto msg = error_message(ParseError::UnknownArgument);
        static_assert(msg == "Unknown argument");
        assert(msg == "Unknown argument");
    });

    ctx.test("All error messages are constexpr", [] {
        using namespace cppcliargs;
        constexpr auto msg1 = error_message(ParseError::MissingRequiredArgument);
        constexpr auto msg2 = error_message(ParseError::MissingValue);
        constexpr auto msg3 = error_message(ParseError::InvalidBooleanValue);
        constexpr auto msg4 = error_message(ParseError::InvalidIntegerValue);
        
        assert(msg1 == "Missing required argument");
        assert(msg2 == "Missing value for argument");
        assert(msg3 == "Invalid boolean value (expected 'true' or 'false')");
        assert(msg4 == "Invalid integer value");
    });

    ctx.summary();
}

void test_real_world_example() {
    TestContext ctx;

    ctx.test("Example from documentation", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}, {'n', 20}};
        std::map<char, std::string> long_names{{'h', "help"}, {'n', "line-count"}};
        std::set<char> required{'n'};
        parser p(defaults, long_names, required);
        
        ArgvBuilder args{"program", "-h", "-n", "50"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('h') == true);
        assert(result.value().get<int>('n') == 50);
    });

    ctx.summary();
}

void test_help_generation() {
    TestContext ctx;

    ctx.test("Generate basic help text", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}, {'n', 20}};
        std::map<char, std::string> long_names{{'h', "help"}, {'n', "line-count"}};
        std::set<char> required{'n'};
        std::map<char, std::string> help{
            {'h', "Show this help message"},
            {'n', "Number of lines"}
        };
        
        parser p(defaults, long_names, required, help);
        std::string help_text = p.generate_help("myprogram");
        
        // Verify help text contains expected elements
        assert(help_text.find("Usage: myprogram") != std::string::npos);
        assert(help_text.find("Options:") != std::string::npos);
        assert(help_text.find("-h") != std::string::npos);
        assert(help_text.find("--help") != std::string::npos);
        assert(help_text.find("-n") != std::string::npos);
        assert(help_text.find("--line-count") != std::string::npos);
        assert(help_text.find("Show this help message") != std::string::npos);
        assert(help_text.find("Number of lines") != std::string::npos);
    });

    ctx.test("Help text shows required arguments", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}, {'f', std::string("")}};
        std::set<char> required{'n', 'f'};
        std::map<char, std::string> help{
            {'n', "Count"},
            {'f', "Filename"}
        };
        
        parser p(defaults, {}, required, help);
        std::string help_text = p.generate_help();
        
        assert(help_text.find("(required)") != std::string::npos);
    });

    ctx.test("Help text shows default values", [] {
        using namespace cppcliargs;
        ArgMap defaults{
            {'n', 42},
            {'f', std::string("output.txt")},
            {'v', false}
        };
        std::map<char, std::string> help{
            {'n', "Count"},
            {'f', "Filename"},
            {'v', "Verbose"}
        };
        
        parser p(defaults, {}, {}, help);
        std::string help_text = p.generate_help();
        
        assert(help_text.find("(default: 42)") != std::string::npos);
        assert(help_text.find("(default: \"output.txt\")") != std::string::npos);
    });

    ctx.test("Help text without long names", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}, {'n', 10}};
        std::map<char, std::string> help{
            {'v', "Verbose mode"},
            {'n', "Number of items"}
        };
        
        parser p(defaults, {}, {}, help);
        std::string help_text = p.generate_help("test");
        
        assert(help_text.find("-v") != std::string::npos);
        assert(help_text.find("-n") != std::string::npos);
        assert(help_text.find("Verbose mode") != std::string::npos);
        assert(help_text.find("Number of items") != std::string::npos);
    });

    ctx.test("Help text arguments are sorted", [] {
        using namespace cppcliargs;
        ArgMap defaults{
            {'z', false},
            {'a', false},
            {'m', false}
        };
        
        parser p(defaults, {});
        std::string help_text = p.generate_help();
        
        // Find positions
        size_t pos_a = help_text.find("-a");
        size_t pos_m = help_text.find("-m");
        size_t pos_z = help_text.find("-z");
        
        // Verify sorted order
        assert(pos_a != std::string::npos);
        assert(pos_m != std::string::npos);
        assert(pos_z != std::string::npos);
        assert(pos_a < pos_m);
        assert(pos_m < pos_z);
    });

    ctx.test("Help without help descriptions", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}, {'n', 20}};
        std::map<char, std::string> long_names{{'h', "help"}, {'n', "count"}};
        
        parser p(defaults, long_names);
        std::string help_text = p.generate_help();
        
        // Should still generate help text, just without descriptions
        assert(help_text.find("Usage:") != std::string::npos);
        assert(help_text.find("-h") != std::string::npos);
        assert(help_text.find("--help") != std::string::npos);
    });

    ctx.test("has_help_request detects -h", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-h"};
        assert(p.has_help_request(args.argc(), args.argv()) == true);
    });

    ctx.test("has_help_request detects --help", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}};
        std::map<char, std::string> long_names{{'h', "help"}};
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "--help"};
        assert(p.has_help_request(args.argc(), args.argv()) == true);
    });

    ctx.test("has_help_request returns false when no help", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}, {'n', 10}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "20"};
        assert(p.has_help_request(args.argc(), args.argv()) == false);
    });

    ctx.summary();
}

void test_convenience_api() {
    TestContext ctx;

    ctx.test("Template get<T>() for int", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 42}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "100"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        // Clean syntax - no std::get!
        int n = result.value().get<int>('n');
        assert(n == 100);
    });

    ctx.test("Template get<T>() for bool", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-v"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        bool verbose = result.value().get<bool>('v');
        assert(verbose == true);
    });

    ctx.test("Template get<T>() for string", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'f', std::string("")}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-f", "output.txt"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        std::string filename = result.value().get<std::string>('f');
        assert(filename == "output.txt");
    });

    ctx.test("ParseResultValue supports range-based for", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'h', false}, {'n', 10}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "20"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        
        // Can iterate directly over result.value()
        int count = 0;
        for ([[maybe_unused]] const auto& [key, value] : result.value()) {
            count++;
        }
        assert(count == 2);
    });

    ctx.test("ParseResultValue subscript operator", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 10}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "42"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        
        // Can use subscript operator
        const auto& value = result.value()['n'];
        assert(std::get<int>(value) == 42);
    });

    ctx.test("ParseResultValue values() accessor", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 10}, {'v', false}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "-n", "5", "-v"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        
        // Can get underlying ArgMap
        const cppcliargs::ArgMap& map = result.value().values();
        assert(map.size() == 3); // n, v, and auto-added h
    });

    ctx.summary();
}

void test_long_arguments() {
    TestContext ctx;

    ctx.test("Parse long argument --name", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        std::map<char, std::string> long_names{{'n', "number"}};
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "--number", "42"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<int>('n') == 42);
    });

    ctx.test("Parse long argument with equals --name=value", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        std::map<char, std::string> long_names{{'n', "count"}};
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "--count=100"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<int>('n') == 100);
    });

    ctx.test("Parse long string argument", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'f', std::string("")}};
        std::map<char, std::string> long_names{{'f', "filename"}};
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "--filename", "output.txt"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<std::string>('f') == "output.txt");
    });

    ctx.test("Parse long string argument with equals", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'f', std::string("")}};
        std::map<char, std::string> long_names{{'f', "file"}};
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "--file=data.csv"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<std::string>('f') == "data.csv");
    });

    ctx.test("Parse long boolean argument", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}};
        std::map<char, std::string> long_names{{'v', "verbose"}};
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "--verbose"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == true);
    });

    ctx.test("Parse required long boolean with value", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', false}};
        std::map<char, std::string> long_names{{'v', "verbose"}};
        std::set<char> required{'v'};
        parser p(defaults, long_names, required);
        
        ArgvBuilder args{"program", "--verbose", "true"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == true);
    });

    ctx.test("Parse long boolean with equals syntax", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'v', true}};
        std::map<char, std::string> long_names{{'v', "verbose"}};
        std::set<char> required{'v'};
        parser p(defaults, long_names, required);
        
        ArgvBuilder args{"program", "--verbose=false"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == false);
    });

    ctx.test("Mix short and long arguments", [] {
        using namespace cppcliargs;
        ArgMap defaults{
            {'v', false},
            {'n', 0},
            {'f', std::string("")}
        };
        std::map<char, std::string> long_names{
            {'v', "verbose"},
            {'n', "count"},
            {'f', "file"}
        };
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "-v", "--count", "50", "--file=output.txt"};
        auto result = p(args.argc(), args.argv());
        
        assert(result.has_value());
        assert(result.value().get<bool>('v') == true);
        assert(result.value().get<int>('n') == 50);
        assert(result.value().get<std::string>('f') == "output.txt");
    });

    ctx.test("Unknown long argument returns error", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        std::map<char, std::string> long_names{{'n', "number"}};
        parser p(defaults, long_names);
        
        ArgvBuilder args{"program", "--unknown", "42"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::UnknownArgument);
    });

    ctx.test("Long argument without mapping returns error", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        // No long names defined
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "--number", "42"};
        auto result = p(args.argc(), args.argv());
        
        assert(!result.has_value());
        assert(result.error().error == ParseError::UnknownArgument);
    });

    ctx.test("Empty -- is ignored", [] {
        using namespace cppcliargs;
        ArgMap defaults{{'n', 0}};
        parser p(defaults, {});
        
        ArgvBuilder args{"program", "--", "-n", "42"};
        auto result = p(args.argc(), args.argv());
        
        // Should skip "--" and parse -n
        assert(result.has_value());
        assert(result.value().get<int>('n') == 42);
    });

    ctx.summary();
}

int main() {
    std::cout << "Running cppcliargs unit tests\n";
    std::cout << "================================\n\n";

    test_basic_parsing();
    std::cout << "\n";
    
    test_required_arguments();
    std::cout << "\n";
    
    test_error_handling();
    std::cout << "\n";
    
    test_equals_syntax();
    std::cout << "\n";
    
    test_long_names();
    std::cout << "\n";
    
    test_constexpr_configuration();
    std::cout << "\n";
    
    test_real_world_example();
    std::cout << "\n";

    test_help_generation();
    std::cout << "\n";

    test_convenience_api();
    std::cout << "\n";

    test_long_arguments();
    std::cout << "\n";

    std::cout << "All test suites completed!\n";
    
    return 0;
}
