# MSVC Linker Warning Fix

## Issue

You may see this warning when building on Windows:

```
LINK : warning LNK4044: unrecognized option '/L\C:\Users\tiit\llvm/lib\'; ignored
```

## Cause

This warning occurs because you have LLVM/Clang installed in a custom location (`C:\Users\tiit\llvm`), and CMake is picking up linker flags from your environment variables or CMake cache.

## Solutions

### Solution 1: Clean CMake Cache (Recommended)

```cmd
cd build
del CMakeCache.txt
del /S /Q CMakeFiles
cmake ..
cmake --build .
```

### Solution 2: Fresh Build Directory

```cmd
rmdir /S /Q build
mkdir build
cd build
cmake ..
cmake --build .
```

### Solution 3: Ignore the Warning

The warning is harmless - it's just CMake trying to pass a Unix-style linker flag (`-L`) to MSVC, which doesn't recognize it. Your executables build correctly despite the warning.

### Solution 4: Set Environment Variables

If you want a permanent fix, ensure your CMake environment is clean:

```cmd
set CMAKE_PREFIX_PATH=
set CMAKE_LIBRARY_PATH=
cmake ..
```

## Why This Happens

The cppcliargs CMakeLists.txt is clean and doesn't add any custom linker paths. The issue comes from:
1. Environment variables (e.g., `CMAKE_LIBRARY_PATH`)
2. CMake cache from previous builds
3. Global CMake configuration

## Verification

After applying the fix, you should only see:

```
Building Custom Rule C:/path/to/cppcliargs_package/CMakeLists.txt
minimal_sum.cpp
minimal_sum.vcxproj -> C:/path/to/build/Debug/minimal_sum.exe
```

No linker warnings!
