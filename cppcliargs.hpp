#pragma once

#include <algorithm>
#include <expected>
#include <map>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <charconv>
#include <system_error>
#include <iostream>

namespace cppcliargs {

// Error types for std::expected
enum class ParseError {
    UnknownArgument,
    MissingRequiredArgument,
    MissingValue,
    InvalidBooleanValue,
    InvalidIntegerValue,
    TypeMismatch,
    DuplicateArgument,
    InvalidArguments
};

// Human-readable error messages
constexpr std::string_view error_message(ParseError error) noexcept {
    switch (error) {
        case ParseError::UnknownArgument: return "Unknown argument";
        case ParseError::MissingRequiredArgument: return "Missing required argument";
        case ParseError::MissingValue: return "Missing value for argument";
        case ParseError::InvalidBooleanValue: return "Invalid boolean value (expected 'true' or 'false')";
        case ParseError::InvalidIntegerValue: return "Invalid integer value";
        case ParseError::TypeMismatch: return "Type mismatch";
        case ParseError::DuplicateArgument: return "Duplicate argument";
        case ParseError::InvalidArguments: return "Invalid arguments";
    }
    return "Unknown error";
}

// Error info structure
struct ParseErrorInfo {
    ParseError error;
    char argument;
    std::string detail;

    std::string to_string() const {
        std::string msg = std::string(error_message(error));
        msg += " for '-";
        msg += argument;
        msg += "'";
        if (!detail.empty()) {
            msg += ": ";
            msg += detail;
        }
        return msg;
    }
};

// Argument value type
using ArgValue = std::variant<int, bool, std::string>;
using ArgMap = std::map<char, ArgValue>;

// Result type with convenience accessors
class ParseResultValue {
public:
    explicit ParseResultValue(ArgMap values) : values_(std::move(values)) {}
    
    // Direct access to the map
    const ArgMap& values() const { return values_; }
    
    // Iterator support for range-based for loops
    auto begin() const { return values_.begin(); }
    auto end() const { return values_.end(); }
    
    // Subscript operator
    const ArgValue& operator[](char key) const { return values_.at(key); }
    const ArgValue& at(char key) const { return values_.at(key); }
    
    // Template-based typed getter
    template<typename T>
    T get(char key) const {
        return std::get<T>(values_.at(key));
    }
    
private:
    ArgMap values_;
};

using ParseResult = std::expected<ParseResultValue, ParseErrorInfo>;

// Configuration structure for parser
struct Config {
    ArgMap defaults;
    std::map<char, std::string> long_names = {};
    std::set<char> required = {};
    std::map<char, std::string> help = {};
};

class parser {
public:
    // Constructor with defaults and argc/argv
    parser(ArgMap defaults, int argc, const char* argv[]) noexcept
        : defaults_(std::move(defaults))
        , argc_(argc)
        , argv_(argv)
    {
        // Always add -h for help if not present
        if (!defaults_.contains('h')) {
            defaults_['h'] = false;
            long_names_['h'] = "help";
        }
        
        // Auto-print help if requested
        if (has_help_request(argc, argv)) {
            std::cout << generate_help(argv[0]);
            help_was_requested_ = true;
        }
    }
    
    // Constructor with full Config and argc/argv
    parser(Config config, int argc, const char* argv[]) noexcept
        : defaults_(std::move(config.defaults))
        , long_names_(std::move(config.long_names))
        , required_(std::move(config.required))
        , help_(std::move(config.help))
        , argc_(argc)
        , argv_(argv)
    {
        // Always add -h for help if not present
        if (!defaults_.contains('h')) {
            defaults_['h'] = false;
            long_names_['h'] = "help";
        }
        
        // Auto-print help if requested
        if (has_help_request(argc, argv)) {
            std::cout << generate_help(argv[0]);
            help_was_requested_ = true;
        }
    }

    // Parse command line arguments using stored argc/argv
    ParseResult operator()() const {
        ArgMap result = defaults_;
        std::set<char> seen_args;

        // Convert to span for modern C++ iteration
        std::span<const char* const> args(argv_, argc_);
        
        // Skip program name - iterate from args.begin() + 1
        for (auto it = args.begin() + 1; it != args.end(); ++it) {
            std::string_view arg(*it);
            
            // Skip non-arguments
            if (arg.empty() || arg[0] != '-') {
                continue;
            }

            char arg_char = '\0';
            std::string_view value_part;
            bool has_equals = false;

            // Parse long argument (--xxx)
            if (arg.size() >= 2 && arg[1] == '-') {
                if (arg.size() == 2) {
                    // Just "--" by itself, skip
                    continue;
                }
                
                std::string_view long_arg = arg.substr(2);
                
                // Check for --arg=value format
                size_t equals_pos = long_arg.find('=');
                if (equals_pos != std::string_view::npos) {
                    std::string_view long_name = long_arg.substr(0, equals_pos);
                    value_part = long_arg.substr(equals_pos + 1);
                    has_equals = true;
                    
                    // Find corresponding short arg
                    arg_char = find_short_for_long(long_name);
                } else {
                    // Just --arg format
                    arg_char = find_short_for_long(long_arg);
                }
                
                if (arg_char == '\0') {
                    return std::unexpected(ParseErrorInfo{
                        ParseError::UnknownArgument,
                        '-',  // Use '-' for unknown long args
                        std::string(arg)
                    });
                }
            }
            // Parse short argument (-x)
            else if (arg.size() >= 2) {
                arg_char = arg[1];
                
                // Check for -x=value format
                if (arg.size() > 2 && arg[2] == '=') {
                    value_part = arg.substr(3);
                    has_equals = true;
                }
            } else {
                // Just "-" by itself, skip
                continue;
            }
            
            // Check if argument is known
            if (!defaults_.contains(arg_char)) {
                return std::unexpected(ParseErrorInfo{
                    ParseError::UnknownArgument,
                    arg_char,
                    std::string(arg)
                });
            }

            // Check for duplicate
            if (seen_args.contains(arg_char)) {
                return std::unexpected(ParseErrorInfo{
                    ParseError::DuplicateArgument,
                    arg_char,
                    ""
                });
            }
            seen_args.insert(arg_char);

            // Handle value based on type
            if (has_equals) {
                // -x=value or --xxx=value format
                auto parse_result = parse_value(arg_char, value_part);
                if (!parse_result) {
                    return std::unexpected(parse_result.error());
                }
                result[arg_char] = *parse_result;
            } else {
                // For bool types
                if (std::holds_alternative<bool>(defaults_.at(arg_char))) {
                    if (required_.contains(arg_char)) {
                        // Required bool must have explicit value
                        if (it + 1 >= args.end()) {
                            return std::unexpected(ParseErrorInfo{
                                ParseError::MissingValue,
                                arg_char,
                                "required boolean needs explicit value"
                            });
                        }
                        ++it;
                        std::string_view bool_value(*it);
                        auto parse_result = parse_value(arg_char, bool_value);
                        if (!parse_result) {
                            return std::unexpected(parse_result.error());
                        }
                        result[arg_char] = *parse_result;
                    } else {
                        // Optional bool, presence means true
                        result[arg_char] = true;
                    }
                } else {
                    // Non-bool types need a value
                    if (it + 1 >= args.end()) {
                        return std::unexpected(ParseErrorInfo{
                            ParseError::MissingValue,
                            arg_char,
                            ""
                        });
                    }
                    ++it;
                    std::string_view next_value(*it);
                    auto parse_result = parse_value(arg_char, next_value);
                    if (!parse_result) {
                        return std::unexpected(parse_result.error());
                    }
                    result[arg_char] = *parse_result;
                }
            }
        }

        // Check all required arguments are present
        for (char req : required_) {
            if (!seen_args.contains(req)) {
                return std::unexpected(ParseErrorInfo{
                    ParseError::MissingRequiredArgument,
                    req,
                    long_names_.contains(req) ? long_names_.at(req) : ""
                });
            }
        }

        return ParseResultValue(std::move(result));
    }
    
    // Check if help was requested (simpler name)
    bool help_requested() const {
        return help_was_requested_;
    }
    
    // NEW: Report error with auto-generated help
    void report_error(const ParseResult& result) const {
        if (!result) {
            std::cerr << "❌ " << result.error().to_string() << "\n\n";
            std::cout << generate_help(argv_ ? argv_[0] : "program");
        }
    }

private:
    // Check if help argument is present (internal use)
    bool has_help_request(int argc, const char* argv[]) const {
        std::span<const char* const> args(argv, argc);
        
        // Skip program name - use range-based for loop with subspan
        for (const char* arg_ptr : args.subspan(1)) {
            std::string_view arg(arg_ptr);
            
            // Check for -h
            if (arg == "-h") {
                return true;
            }
            
            // Check for --help if 'h' has "help" as long name
            if (long_names_.contains('h') && long_names_.at('h') == "help") {
                if (arg == "--help") {
                    return true;
                }
            }
        }
        
        return false;
    }

    // Find short argument character for a long name
    char find_short_for_long(std::string_view long_name) const {
        for (const auto& [key, name] : long_names_) {
            if (name == long_name) {
                return key;
            }
        }
        return '\0';  // Not found
    }
    // Parse a value based on the type in defaults
    std::expected<ArgValue, ParseErrorInfo> parse_value(char arg_char, std::string_view value) const {
        const ArgValue& default_val = defaults_.at(arg_char);
        
        if (std::holds_alternative<bool>(default_val)) {
            if (value == "true") {
                return true;
            } else if (value == "false") {
                return false;
            }
            return std::unexpected(ParseErrorInfo{
                ParseError::InvalidBooleanValue,
                arg_char,
                std::string(value)
            });
        } else if (std::holds_alternative<int>(default_val)) {
            int result;
            auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);
            if (ec != std::errc{} || ptr != value.data() + value.size()) {
                return std::unexpected(ParseErrorInfo{
                    ParseError::InvalidIntegerValue,
                    arg_char,
                    std::string(value)
                });
            }
            return result;
        } else if (std::holds_alternative<std::string>(default_val)) {
            return std::string(value);
        }

        return std::unexpected(ParseErrorInfo{
            ParseError::TypeMismatch,
            arg_char,
            ""
        });
    }

    ArgMap defaults_;
    std::map<char, std::string> long_names_;
    std::set<char> required_;
    std::map<char, std::string> help_;
    
    // Stored command line arguments (when using improved constructor)
    int argc_ = 0;
    const char** argv_ = nullptr;
    mutable bool help_was_requested_ = false;

public:
    // Generate help text
    std::string generate_help(const std::string& program_name = "program") const {
        std::string result;
        result += "Usage: " + program_name + " [OPTIONS]\n\n";
        result += "Options:\n";
        
        // Collect all argument characters and sort them
        std::vector<char> args;
        for (const auto& [key, _] : defaults_) {
            args.push_back(key);
        }
        std::sort(args.begin(), args.end());
        
        // Generate help line for each argument
        for (char arg : args) {
            result += "  -";
            result += arg;
            
            // Add long name if present
            if (long_names_.contains(arg)) {
                result += ", --";
                result += long_names_.at(arg);
                
                // Pad to align descriptions
                size_t current_length = 6 + long_names_.at(arg).length(); // "  -x, --" + name
                if (current_length < 28) {
                    result += std::string(28 - current_length, ' ');
                }
            } else {
                // No long name, just pad after short option
                result += std::string(24, ' ');
            }
            
            // Add help text if present, otherwise show type
            if (help_.contains(arg)) {
                result += help_.at(arg);
            } else {
                // Show type when no help text provided
                const auto& default_val = defaults_.at(arg);
                if (std::holds_alternative<int>(default_val)) {
                    result += "[integer]";
                } else if (std::holds_alternative<bool>(default_val)) {
                    result += "[boolean]";
                } else if (std::holds_alternative<std::string>(default_val)) {
                    result += "[string]";
                }
            }
            
            // Add default value
            const auto& default_val = defaults_.at(arg);
            
            if (std::holds_alternative<int>(default_val)) {
                int def = std::get<int>(default_val);
                if (!required_.contains(arg)) {
                    result += " (default: " + std::to_string(def) + ")";
                } else {
                    result += " (required)";
                }
            } else if (std::holds_alternative<bool>(default_val)) {
                if (required_.contains(arg)) {
                    result += " (required)";
                }
            } else if (std::holds_alternative<std::string>(default_val)) {
                const auto& def = std::get<std::string>(default_val);
                if (!required_.contains(arg)) {
                    if (!def.empty()) {
                        result += " (default: \"" + def + "\")";
                    }
                } else {
                    result += " (required)";
                }
            }
            
            result += "\n";
        }
        
        return result;
    }
};

} // namespace cppcliargs
