# cppcliargs - Modern C++23 Command Line Parser

A lightweight, header-only command line argument parser with modern C++ features.

## Features

- ✨ **Header-only** - Just include `cppcliargs.hpp`
- 🔒 **Type-safe** - `std::variant<int, bool, std::string>` with `std::expected`
- 🎯 **Modern C++23** - Uses `std::expected`, `std::span`, `std::string_view`
- 🚀 **Zero dependencies** - Only standard library
- 📖 **Auto-generated help** - Built-in help text
- 🎨 **Long arguments** - Both `-n` and `--name` supported
- 💎 **Clean API** - Template-based `get<T>()` accessors
- 🧪 **Well tested** - 53 comprehensive tests

# cppcliargs

[![CMake Multi-Platform](https://github.com/asjadenet/cppcliargs/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/asjadenet/cppcliargs/actions/workflows/cmake-multi-platform.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

Modern C++23 command-line argument parser - type-safe, exception-free, header-only.

## Quick Start

```cpp
#include "cppcliargs.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
    // Configure with designated initializers - clear and explicit!
    cppcliargs::Config config{
        .defaults = {
            {'n', 20},
            {'f', "output.txt"}  // String literals work directly!
        },
        .long_names = {
            {'n', "count"}, 
            {'f', "file"}
        },
        .required = {'n'},
        .help = {
            {'n', "Number of items to process"},
            {'f', "Output file"}
        }
    };
    
    cppcliargs::parser p(config);
    
    // Check for help request
    if (p.has_help_request(argc, argv)) {
        std::cout << p.generate_help(argv[0]);
        return 0;
    }
    
    // Parse arguments
    auto result = p(argc, argv);
    if (!result) {
        std::cerr << "Error: " << result.error().to_string() << "\n\n";
        std::cout << p.generate_help(argv[0]);
        return 1;
    }
    
    // Clean value access
    const auto& values = result.value();
    int count = values.get<int>('n');
    std::string filename = values.get<std::string>('f');
    
    std::cout << "Processing " << count << " items to " << filename << "\n";
    return 0;
}
```

**Note:** String literals (`"text"`) are automatically converted to `std::string`. The `-h` / `--help` argument is added automatically!

## Building

### Simple Compilation (No CMake)

```bash
# Linux/macOS
g++ -std=c++23 -O2 your_program.cpp -o your_program

# Windows
cl /std:c++latest /EHsc /O2 your_program.cpp
```

### With CMake

**Build examples and tests:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest  # Run tests
```

**Windows:**
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
ctest -C Release
```

### Install System-Wide

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build .
sudo cmake --install .
```

Then in your project:
```cmake
find_package(cppcliargs 1.0 REQUIRED)
target_link_libraries(your_app PRIVATE cppcliargs::cppcliargs)
```

## Usage Examples

### Supported Syntax

```bash
# Short arguments
program -n 42 -f output.txt -v

# Long arguments  
program --count 42 --file output.txt --verbose

# Equals syntax
program -n=42 --file=output.txt

# Mixed
program -n 42 --file=output.txt -v
```

### Value Access

```cpp
auto result = parser_inst(argc, argv);
if (result) {
    const auto& values = result.value();
    
    // Template-based (recommended)
    int n = values.get<int>('n');
    bool v = values.get<bool>('v');
    std::string f = values.get<std::string>('f');
    
    // Iterate all values
    for (const auto& [key, value] : values) {
        // Process each argument
    }
}
```

### Error Handling

```cpp
auto result = parser_inst(argc, argv);
if (!result) {
    const auto& error = result.error();
    std::cerr << "Error: " << error.to_string() << "\n";
    // error.error contains the error type
    // error.argument contains the problematic argument
    // error.detail contains additional info
    return 1;
}
```

### Auto-Generated Help

```cpp
cppcliargs::parser p(args, long_names, required, help);
std::cout << p.generate_help("myprogram");
```

Output:
```
Usage: myprogram [OPTIONS]

Options:
  -f, --file                  Output file (default: "output.txt")
  -h, --help                  Show this help message
  -n, --count                 Number of items to process (required)
  -v, --verbose               Enable verbose output
```

## API Reference

### Core Types

```cpp
cppcliargs::ArgMap              // std::map<char, std::variant<int, bool, std::string>>
cppcliargs::Config              // Configuration struct
cppcliargs::ParseResultValue    // Result wrapper with get<T>() methods
cppcliargs::ParseResult         // std::expected<ParseResultValue, ParseErrorInfo>
```

### Config Struct

```cpp
struct Config {
    ArgMap defaults;                        // Argument defaults (required)
    std::map<char, std::string> long_names; // Long argument names (optional)
    std::set<char> required;                // Required arguments (optional)
    std::map<char, std::string> help;       // Help text (optional)
    bool auto_help = true;                  // Auto-add -h/--help (optional)
};
```

**Usage with designated initializers:**
```cpp
cppcliargs::Config config{
    .defaults = {{'n', 10}},
    .long_names = {{'n', "count"}},
    .required = {'n'},
    .help = {{'n', "Item count"}},
    .auto_help = true
};
```

### Parser Class

```cpp
// Recommended: Config struct
cppcliargs::Config config{...};
cppcliargs::parser p(config);

// Also available: Convenience constructors
cppcliargs::parser p(defaults);                              // Simple
cppcliargs::parser p(defaults, long_names, required, help);  // Full

// Methods:
ParseResult operator()(int argc, const char* argv[]) const;
bool has_help_request(int argc, const char* argv[]) const;
std::string generate_help(const std::string& program_name) const;
```

**Note:** While convenience constructors exist for compatibility, the Config struct approach is recommended for new code as it's more explicit and type-safe.

### ParseResultValue Methods

```cpp
template<typename T> T get(char key) const;     // Recommended accessor
const ArgValue& operator[](char key) const;     // Direct access
const ArgValue& at(char key) const;             // Bounds-checked
const ArgMap& values() const;                   // Underlying map
// Range-based for loop support via begin()/end()
```

## Configuration

### Using Designated Initializers (Recommended)

```cpp
cppcliargs::Config config{
    .defaults = {
        {'n', 10},
        {'f', "out.txt"}  // String literals work directly!
    },
    .long_names = {
        {'n', "count"}
    },
    .required = {'n'},
    .help = {
        {'n', "Item count"}
    }
};

cppcliargs::parser p(config);
```

**Benefits:**
- ✅ Field names are explicit - no confusion about order
- ✅ String literals automatically convert to `std::string`
- ✅ Can skip optional fields (they use defaults)
- ✅ Easy to read and maintain

### Minimal Example

```cpp
// Only specify what you need
cppcliargs::Config config{
    .defaults = {{'n', 10}, {'v', false}, {'f', "data.txt"}},
    .required = {'n'}
};

cppcliargs::parser p(config);
```

**Automatic Help:** The parser automatically adds `-h` / `--help` unless:
- You already defined 'h' in your arguments
- You set `.auto_help = false`

## Requirements

- **C++23 compiler:**
  - GCC 13+
  - Clang 16+
  - MSVC 2022 17.8+
- **CMake 3.20+** (optional, only for building examples/tests)

## Testing

Includes 53 comprehensive tests covering:
- Argument parsing (short/long forms)
- Required arguments
- Error handling
- Type validation
- Help generation
- Template accessors

Run tests:
```bash
./test_cppcliargs    # After building
```

## Examples Included

- `simple_example.cpp` - Basic usage
- `modern_example.cpp` - Clean API demonstration
- `advanced_example.cpp` - Full-featured with validation

Build and run:
```bash
cmake --build .
./simple_example --help
./modern_example --count=42 --verbose
```

## CMake Options

```bash
# Build control
-DCPPCLIARGS_BUILD_EXAMPLES=ON/OFF  # Default: ON
-DCPPCLIARGS_BUILD_TESTS=ON/OFF     # Default: ON

# Installation
-DCMAKE_INSTALL_PREFIX=/path        # Install location

# Build type
-DCMAKE_BUILD_TYPE=Release          # Release/Debug
```

## Troubleshooting

**"C++23 not supported"**
- Update compiler to GCC 13+, Clang 16+, or MSVC 2022 17.8+

**"std::expected not found"**
- Ensure C++23 mode: `-std=c++23` or `/std:c++latest`

**"cmake directory missing"** (Windows ZIP download)
- Ignore it - CMake auto-generates needed files
- Or manually create: `mkdir cmake` (not required for building)

**CMake can't find package after install**
```bash
cmake .. -Dcppcliargs_DIR=/install/path/lib/cmake/cppcliargs
```

## Integration Methods

### 1. Direct Copy (Simplest)
Just copy `cppcliargs.hpp` to your project.

### 2. CMake find_package (After install)
```cmake
find_package(cppcliargs 1.0 REQUIRED)
target_link_libraries(app PRIVATE cppcliargs::cppcliargs)
```

### 3. CMake FetchContent (No install)
```cmake
include(FetchContent)
FetchContent_Declare(cppcliargs GIT_REPOSITORY ... GIT_TAG v1.0.0)
FetchContent_MakeAvailable(cppcliargs)
target_link_libraries(app PRIVATE cppcliargs::cppcliargs)
```

### 4. Git Submodule
```cmake
add_subdirectory(external/cppcliargs)
target_link_libraries(app PRIVATE cppcliargs::cppcliargs)
```

## License

Use freely in your projects.

## Support

- Check examples for working code
- Read API reference above
- Run tests to verify installation
- All features are demonstrated in example files
