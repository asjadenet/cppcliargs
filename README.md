# cppcliargs

Modern C++23 command line argument parser - simple, type-safe, and powerful.

[![CMake Multi-Platform](https://github.com/asjadenet/cppcliargs/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/asjadenet/cppcliargs/actions/workflows/cmake-multi-platform.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)](cppcliargs.hpp)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## Features

- ✨ **Header-only** - Just include `cppcliargs.hpp`
- 🔒 **Type-safe** - `std::variant<int, bool, std::string>` with `std::expected`
- 🎯 **Modern C++23** - Uses `std::expected`, designated initializers
- 🚀 **Zero dependencies** - Only standard library
- 📖 **Auto-help** - Automatically printed when `-h` is used
- 💎 **Clean API** - Pass `argc/argv` once, use everywhere
- 🎨 **Long arguments** - Both `-n` and `--count` supported
- 🧪 **Type display** - Shows `[integer]`, `[boolean]`, `[string]` in help

## Quick Start

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
    std::cout << a + b << "\n";
}
```

**Compile and run:**
```bash
g++ -std=c++23 example.cpp -o example

./example -a 5 -b 3
# Output: 8

./example -h
# Shows auto-generated help with types

./example -a invalid
# ❌ Invalid integer value for '-a': invalid
# [shows help automatically]
```

## Installation

### Header-only

Just copy `cppcliargs.hpp` to your project:

```bash
wget https://raw.githubusercontent.com/asjadenet/cppcliargs/main/cppcliargs.hpp
```

### CMake - FetchContent (Recommended)

Add to your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Fetch cppcliargs
include(FetchContent)
FetchContent_Declare(
    cppcliargs
    GIT_REPOSITORY https://github.com/asjadenet/cppcliargs.git
    GIT_TAG main
)
FetchContent_MakeAvailable(cppcliargs)

# Your executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE cppcliargs::cppcliargs)
target_compile_features(myapp PRIVATE cxx_std_23)
```

### CMake - Installed Package

If you've installed cppcliargs system-wide:

```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Find installed cppcliargs
find_package(cppcliargs REQUIRED)

# Your executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE cppcliargs::cppcliargs)
target_compile_features(myapp PRIVATE cxx_std_23)
```

**Install cppcliargs:**
```bash
cd cppcliargs
mkdir build && cd build
cmake ..
sudo cmake --install .
```

### CMake - Subdirectory

If cppcliargs is in your project:

```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Add cppcliargs subdirectory
add_subdirectory(external/cppcliargs)

# Your executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE cppcliargs::cppcliargs)
target_compile_features(myapp PRIVATE cxx_std_23)
```

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
    std::cout << a + b << "\n";
}
```

### With Long Names and Help Text

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
    
    if (p.help_requested()) return 0;
    
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
}
```

**Usage:**
```bash
./app -n 100 -f input.txt
./app --count 100 --file input.txt --verbose
./app -n 100 -f input.txt -v --threads 8
```

## API Reference

### Constructors

```cpp
// Simple constructor
parser(ArgMap defaults, int argc, const char* argv[])

// Full constructor with Config
parser(Config config, int argc, const char* argv[])
```

**Behavior:**
- Stores `argc` and `argv` for later use
- Automatically adds `-h`/`--help` flag
- Auto-prints help if `-h` is specified

### Methods

```cpp
bool help_requested() const
```
Returns `true` if help was requested and printed.

```cpp
ParseResult operator()() const
```
Parses arguments using stored `argc`/`argv`.  
Returns `std::expected<ParseResultValue, ParseErrorInfo>`.

```cpp
void report_error(const ParseResult& result) const
```
Prints error message with emoji and auto-generated help.

```cpp
std::string generate_help(const std::string& program_name = "program") const
```
Generates help text (rarely needed - `report_error()` calls this automatically).

### Types

```cpp
// Value types
using ArgValue = std::variant<int, bool, std::string>;
using ArgMap = std::map<char, ArgValue>;

// Configuration
struct Config {
    ArgMap defaults;                        // Required
    std::map<char, std::string> long_names; // Optional
    std::set<char> required;                // Optional
    std::map<char, std::string> help;       // Optional
};

// Result types
using ParseResult = std::expected<ParseResultValue, ParseErrorInfo>;

// Access parsed values
class ParseResultValue {
    template<typename T>
    T get(char arg) const;  // Get typed value
    
    const ArgMap& values() const;  // Get all values
};
```

### Error Types

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

## Features in Detail

### Automatic Help

Every parser gets `-h` and `--help` automatically:

```cpp
const cppcliargs::parser p({{'a', 0}}, argc, argv);

if (p.help_requested()) return 0;  // Help already printed!
```

### Type Display

Help shows types even without help text:

```
Usage: ./app [OPTIONS]

Options:
  -a                        [integer] (default: 0)
  -b                        [boolean]
  -f                        [string] (default: "")
  -h, --help                [boolean]
```

### Error Reporting

One-line error reporting with automatic help:

```cpp
if (!result) {
    p.report_error(result);  // Prints error + help
    return 1;
}
```

Output:
```
❌ Invalid integer value for '-a': bad

Usage: ./app [OPTIONS]
...
```

### Required Arguments

```cpp
const cppcliargs::Config config{
    .defaults = {{'n', 0}, {'f', ""}},
    .required = {'n', 'f'}
};
```

Missing required arguments are automatically detected.

### Long Argument Names

```cpp
.long_names = {
    {'v', "verbose"},
    {'n', "count"}
}
```

Both forms work:
```bash
./app -v -n 100
./app --verbose --count 100
```

## Advanced Usage

### Accessing All Values

```cpp
const auto& values = result.value();

// Iterate over all arguments
for (const auto& [key, value] : values) {
    std::cout << "Argument '" << key << "': ";
    std::visit([](const auto& v) { std::cout << v; }, value);
    std::cout << "\n";
}
```

### Boolean Flags

```cpp
const cppcliargs::parser p({{'v', false}}, argc, argv);

const auto result = p();
const bool verbose = result.value().get<bool>('v');

// Usage: ./app -v  (sets verbose to true)
```

### String Arguments

```cpp
const cppcliargs::parser p({{'f', "default.txt"}}, argc, argv);

const auto result = p();
const auto file = result.value().get<std::string>('f');

// Usage: ./app -f input.txt
```

## Best Practices

1. **Always check help first:**
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

## Comparison with Other Libraries

| Feature | cppcliargs | CLI11 | cxxopts | Boost.Program_options |
|---------|------------|-------|---------|----------------------|
| Header-only | ✅ | ✅ | ✅ | ❌ |
| C++23 | ✅ | ❌ | ❌ | ❌ |
| std::expected | ✅ | ❌ | ❌ | ❌ |
| Auto-help printing | ✅ | ❌ | ❌ | ❌ |
| Pass argc/argv once | ✅ | ❌ | ❌ | ❌ |
| Lines of code (simple) | 10 | 12 | 15 | 18 |
| Designated initializers | ✅ | ❌ | ❌ | ❌ |

## Requirements

- **C++23** compiler (GCC 13+, Clang 18+, MSVC 19.35+)
- Standard library with `<expected>` support

### Tested Compilers

- GCC 13+ on Linux
- Clang 18+ (with libc++) on Linux
- MSVC 19.35+ on Windows
- Apple Clang 15+ on macOS

## Building Examples

### Linux/macOS - GCC/Clang

```bash
# Using CMake
mkdir build && cd build
cmake ..
cmake --build .

# Manual compilation
g++ -std=c++23 minimal_sum.cpp -o minimal_sum
./minimal_sum -a 10 -b 20
```

### Windows - Visual Studio (MSVC)

**Note:** Open "Developer Command Prompt for VS" to ensure compiler is in PATH.

```cmd
REM Using CMake
mkdir build
cd build
cmake ..
cmake --build . --config Release

REM Run example
Release\minimal_sum.exe -a 10 -b 20
```

**Manual compilation:**
```cmd
REM Open "Developer Command Prompt for VS" first
cl /std:c++latest /EHsc minimal_sum.cpp
minimal_sum.exe -a 10 -b 20
```

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Acknowledgments

- Uses C++23 features: `std::expected`, `std::span`, designated initializers
- Inspired by modern CLI parsing libraries
- Built for manufacturing and SAP integration workflows
