# API Reference - cppcliargs

Modern C++23 command line argument parser with automatic help and error reporting.

## Table of Contents
- [Quick Start](#quick-start)
- [Constructors](#constructors)
- [Methods](#methods)
- [Types](#types)
- [Examples](#examples)

## Quick Start

```cpp
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
```

## Constructors

### Simple Constructor

```cpp
parser(ArgMap defaults, int argc, const char* argv[])
```

Creates a parser with just defaults and command line arguments.

**Parameters:**
- `defaults` - Map of argument characters to default values
- `argc` - Argument count from main()
- `argv` - Argument vector from main()

**Example:**
```cpp
const cppcliargs::parser p({{'a', 0}, {'b', 0}}, argc, argv);
```

**Behavior:**
- Automatically adds `-h` (help) flag if not present
- Auto-prints help if `-h` or `--help` is specified
- Sets `help_requested()` flag

### Full Constructor

```cpp
parser(Config config, int argc, const char* argv[])
```

Creates a parser with full configuration.

**Parameters:**
- `config` - Configuration struct (see [Config](#config-struct))
- `argc` - Argument count from main()
- `argv` - Argument vector from main()

**Example:**
```cpp
const cppcliargs::Config config{
    .defaults = {{'v', false}, {'n', 0}},
    .long_names = {{'v', "verbose"}, {'n', "count"}},
    .required = {'n'},
    .help = {
        {'v', "Enable verbose output"},
        {'n', "Number of iterations"}
    }
};

const cppcliargs::parser p(config, argc, argv);
```

## Methods

### operator()()

```cpp
ParseResult operator()() const
```

Parses command line arguments using the argc/argv provided to constructor.

**Returns:** `std::expected<ParseResultValue, ParseErrorInfo>`

**Example:**
```cpp
const auto result = p();
if (result) {
    // Success - use result.value()
} else {
    // Error - use result.error()
}
```

### help_requested()

```cpp
bool help_requested() const
```

Returns `true` if `-h` or `--help` was specified on the command line.

**Note:** Help is automatically printed when this is true!

**Example:**
```cpp
if (p.help_requested()) {
    return 0;  // Help already printed, just exit
}
```

### report_error()

```cpp
void report_error(const ParseResult& result) const
```

Prints error message with emoji and auto-generated help text.

**Parameters:**
- `result` - The ParseResult containing the error

**Output format:**
```
❌ Invalid integer value for '-a': bad

Usage: ./program [OPTIONS]
...
```

**Example:**
```cpp
const auto result = p();
if (!result) {
    p.report_error(result);
    return 1;
}
```

### generate_help()

```cpp
std::string generate_help(const std::string& program_name = "program") const
```

Generates help text string (rarely needed - use `report_error()` instead).

**Parameters:**
- `program_name` - Name to show in usage line

**Returns:** Formatted help string

**Example:**
```cpp
const std::string help = p.generate_help("myapp");
std::cout << help;
```

## Types

### ArgMap

```cpp
using ArgValue = std::variant<int, bool, std::string>;
using ArgMap = std::map<char, ArgValue>;
```

Map of argument characters to their values.

**Supported types:**
- `int` - Integer arguments
- `bool` - Boolean flags
- `std::string` - String arguments

**Example:**
```cpp
const cppcliargs::ArgMap defaults{
    {'n', 0},           // int
    {'v', false},       // bool
    {'f', "out.txt"}    // string
};
```

### Config Struct

```cpp
struct Config {
    ArgMap defaults;
    std::map<char, std::string> long_names = {};
    std::set<char> required = {};
    std::map<char, std::string> help = {};
};
```

Configuration for the parser.

**Fields:**
- `defaults` - Default values for all arguments (required)
- `long_names` - Map of short names to long names (optional)
- `required` - Set of required argument characters (optional)
- `help` - Help text for each argument (optional)

**Example:**
```cpp
const cppcliargs::Config config{
    .defaults = {
        {'v', false},
        {'n', 0},
        {'f', ""}
    },
    .long_names = {
        {'v', "verbose"},
        {'n', "count"},
        {'f', "file"}
    },
    .required = {'n', 'f'},
    .help = {
        {'v', "Enable verbose output"},
        {'n', "Number of iterations"},
        {'f', "Input filename"}
    }
};
```

### ParseResult

```cpp
using ParseResult = std::expected<ParseResultValue, ParseErrorInfo>;
```

Result of parsing - either success with values or error.

**Usage:**
```cpp
const auto result = p();

if (result) {
    // Success
    const auto& values = result.value();
    const int n = values.get<int>('n');
} else {
    // Error
    const auto& error = result.error();
    std::cerr << error.to_string() << "\n";
}
```

### ParseResultValue

```cpp
class ParseResultValue {
    template<typename T>
    T get(char arg) const;
    
    const ArgMap& values() const;
    ArgValue operator[](char arg) const;
};
```

Container for successfully parsed values.

**Methods:**
- `get<T>(char)` - Get typed value for argument
- `values()` - Get underlying ArgMap
- `operator[]` - Get ArgValue for argument

**Example:**
```cpp
const auto& values = result.value();

const int n = values.get<int>('n');
const bool v = values.get<bool>('v');
const auto f = values.get<std::string>('f');
```

### ParseErrorInfo

```cpp
struct ParseErrorInfo {
    ParseError error;
    char argument;
    std::string detail;
    
    std::string to_string() const;
};
```

Error information from failed parsing.

**Example:**
```cpp
if (!result) {
    const auto& error = result.error();
    std::cerr << error.to_string() << "\n";
    // Output: "Invalid integer value for '-a': bad"
}
```

### ParseError

```cpp
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
```

Error types that can occur during parsing.

## Examples

### Minimal Example

```cpp
#include "cppcliargs.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
    const cppcliargs::parser p({{'a', 0}, {'b', 0}}, argc, argv);
    
    if (p.help_requested()) return 0;
    
    const auto result = p();
    if (!result) {
        p.report_error(result);
        return 1;
    }
    
    const int a = result.value().get<int>('a');
    const int b = result.value().get<int>('b');
    std::cout << a << " + " << b << " = " << (a + b) << "\n";
}
```

### Full-Featured Example

```cpp
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
    
    const auto& values = result.value();
    const bool verbose = values.get<bool>('v');
    const int count = values.get<int>('n');
    const auto file = values.get<std::string>('f');
    const int threads = values.get<int>('t');
    
    if (verbose) {
        std::cout << "Processing " << file << " with " << count 
                  << " iterations using " << threads << " threads\n";
    }
    
    // Process...
}
```

### Usage Examples

```bash
# Show help
./app -h
./app --help

# Simple arguments
./app -n 100 -f input.txt

# Long names
./app --count 100 --file input.txt --verbose

# Mixed
./app -n 100 --file input.txt -v

# With defaults
./app -n 100 -f input.txt
# Uses default: threads=4

# Error handling
./app -n invalid
# Output:
# ❌ Invalid integer value for '-n': invalid
# 
# Usage: ./app [OPTIONS]
# ...
```

## Best Practices

1. **Always check help_requested() first:**
   ```cpp
   if (p.help_requested()) return 0;
   ```

2. **Always check parsing result:**
   ```cpp
   const auto result = p();
   if (!result) {
       p.report_error(result);
       return 1;
   }
   ```

3. **Use const everywhere:**
   ```cpp
   const auto& values = result.value();
   const int a = values.get<int>('a');
   const int b = values.get<int>('b');
   ```

4. **Use Config for complex apps:**
   ```cpp
   const cppcliargs::Config config{
       .defaults = {...},
       .long_names = {...},
       .required = {...},
       .help = {...}
   };
   ```

## Features

- ✅ **Automatic `-h` help** - Always available
- ✅ **Auto-print help** - Printed when `-h` is used
- ✅ **Type safety** - `std::variant` + `std::expected`
- ✅ **Error reporting** - Clear messages with `report_error()`
- ✅ **Long names** - Both `-n` and `--count` supported
- ✅ **Required args** - Automatic validation
- ✅ **Type display** - Shows `[integer]`, `[boolean]`, `[string]` in help
- ✅ **Header-only** - Just include `cppcliargs.hpp`
- ✅ **C++23** - Modern features

## Notes

- `-h` and `--help` are automatically added to all parsers
- Help is automatically printed when `-h` is specified
- `argc` and `argv` are stored in the constructor
- Parsing uses stored `argc`/`argv` via `operator()()`
- Error reporting includes help text automatically
- Use `const` everywhere for best practices
