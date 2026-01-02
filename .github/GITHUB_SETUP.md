# GitHub Repository Setup Guide

This guide will help you set up cppcliargs as a GitHub repository with automated CI/CD.

## Initial Repository Setup

### 1. Create GitHub Repository

```bash
# Initialize git repository
cd cppcliargs_package
git init

# Create .gitignore
cat > .gitignore << 'EOF'
# Build directories
build/
Build/
cmake-build-*/

# IDE files
.vscode/
.idea/
*.swp
*.swo
*~

# Compiled files
*.exe
*.out
*.o
*.obj
*.so
*.dll
*.dylib

# CMake cache
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
install_manifest.txt

# Test outputs
Testing/
CTestTestfile.cmake

# Package files (keep source)
*.zip
*.tar.gz
EOF

# Add files
git add .
git commit -m "Initial commit: cppcliargs - Modern C++23 CLI parser"

# Add remote (replace with your GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/cppcliargs.git
git branch -M main
git push -u origin main
```

### 2. Enable GitHub Actions

The workflow file is already in `.github/workflows/cmake-multi-platform.yml`

GitHub Actions will automatically:
- ✅ Build on Ubuntu, Windows, and macOS
- ✅ Test with GCC 13, Clang 16, and MSVC
- ✅ Run all 53 unit tests
- ✅ Test all example programs

### 3. Add Status Badge to README

Add this to the top of your README.md:

```markdown
# cppcliargs

[![CMake Multi-Platform](https://github.com/YOUR_USERNAME/cppcliargs/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/YOUR_USERNAME/cppcliargs/actions/workflows/cmake-multi-platform.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

Modern C++23 command-line argument parser - type-safe, exception-free, header-only.
```

## GitHub Actions Workflow Details

### Platforms Tested

| OS | Compiler | C++ Version |
|----|----------|-------------|
| Ubuntu 22.04 | GCC 13 | C++23 |
| Ubuntu 22.04 | Clang 16 | C++23 |
| Windows 2022 | MSVC 19.4+ | C++23 |
| macOS 13 | AppleClang | C++23 |

### What Gets Tested

1. **Compilation**
   - All source files compile without warnings
   - Examples build successfully
   - Tests build successfully

2. **Unit Tests**
   - 53 comprehensive tests via CTest
   - All argument parsing scenarios
   - Error handling validation

3. **Example Programs**
   - `simple_example` - Basic usage
   - `modern_example` - Designated initializers
   - `advanced_example` - Full features

### Workflow Triggers

- **Push** to `main` or `master` branch
- **Pull requests** to `main` or `master` branch

## Troubleshooting

### If workflow fails on Ubuntu with GCC

Check that GCC 13 is available:
```yaml
- name: Install GCC 13 (Ubuntu)
  if: matrix.os == 'ubuntu-latest' && matrix.compiler == 'gcc'
  run: |
    sudo apt-get update
    sudo apt-get install -y g++-13
```

### If workflow fails on Ubuntu with Clang

Check that Clang 16 is available:
```yaml
- name: Install Clang 16 (Ubuntu)
  if: matrix.os == 'ubuntu-latest' && matrix.compiler == 'clang'
  run: |
    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
    sudo ./llvm.sh 16
```

### If workflow fails on Windows

MSVC should have C++23 support in the latest Visual Studio 2022 (17.8+).
The workflow uses the default MSVC on `windows-latest`.

### If workflow fails on macOS

AppleClang 15+ should support most C++23 features.
If you need specific C++23 features not yet in AppleClang, you can:
1. Install GCC via Homebrew
2. Install LLVM Clang via Homebrew

## Optional: Add More Badges

### Code Coverage (if you add coverage)
```markdown
[![codecov](https://codecov.io/gh/YOUR_USERNAME/cppcliargs/branch/main/graph/badge.svg)](https://codecov.io/gh/YOUR_USERNAME/cppcliargs)
```

### Release Version
```markdown
[![Release](https://img.shields.io/github/v/release/YOUR_USERNAME/cppcliargs)](https://github.com/YOUR_USERNAME/cppcliargs/releases)
```

### GitHub Stars
```markdown
[![GitHub stars](https://img.shields.io/github/stars/YOUR_USERNAME/cppcliargs)](https://github.com/YOUR_USERNAME/cppcliargs/stargazers)
```

## Repository Settings

### Recommended Settings

1. **Branch Protection** (Settings → Branches)
   - Require status checks to pass before merging
   - Require branches to be up to date before merging
   - Check: "CMake Multi-Platform Build"

2. **Actions** (Settings → Actions)
   - Allow all actions and reusable workflows
   - Required workflows: cmake-multi-platform.yml

3. **Topics** (Main page)
   Add these topics to help people find your library:
   - cpp23
   - cpp
   - command-line
   - cli-parser
   - argument-parser
   - header-only
   - type-safe
   - modern-cpp
   - cmake

## License

Don't forget to add a LICENSE file! Popular choices:

### MIT License (Permissive)
```bash
cat > LICENSE << 'EOF'
MIT License

Copyright (c) 2024 [Your Name]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
EOF
```

## Next Steps

1. ✅ Push code to GitHub
2. ✅ Verify GitHub Actions runs successfully
3. ✅ Add status badge to README
4. ✅ Add LICENSE file
5. ✅ Add repository topics
6. ✅ Write CONTRIBUTING.md (optional)
7. ✅ Create first release (v1.0.0)

Congratulations! Your library is now professionally set up on GitHub! 🎉
