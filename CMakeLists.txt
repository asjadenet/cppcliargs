cmake_minimum_required(VERSION 3.20)
project(cppcliargs 
    VERSION 1.0.0
    DESCRIPTION "Modern C++23 command line argument parser"
    LANGUAGES CXX
)

# Note: For full installation support with find_package(), create:
#   cmake/cppcliargs-config.cmake.in
# If missing, a minimal config will be auto-generated.

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only library interface
add_library(cppcliargs INTERFACE)
add_library(cppcliargs::cppcliargs ALIAS cppcliargs)

target_include_directories(cppcliargs INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Compiler warnings
if(MSVC)
    set(WARNING_FLAGS /W4 /WX)
else()
    set(WARNING_FLAGS -Wall -Wextra -pedantic -Werror)
endif()

# Option to build examples and tests
option(CPPCLIARGS_BUILD_EXAMPLES "Build example programs" ON)
option(CPPCLIARGS_BUILD_TESTS "Build test suite" ON)

# Examples
if(CPPCLIARGS_BUILD_EXAMPLES)
    # Simple example
    add_executable(simple_example simple_example.cpp)
    target_link_libraries(simple_example PRIVATE cppcliargs::cppcliargs)
    target_compile_options(simple_example PRIVATE ${WARNING_FLAGS})

    # Modern example
    add_executable(modern_example modern_example.cpp)
    target_link_libraries(modern_example PRIVATE cppcliargs::cppcliargs)
    target_compile_options(modern_example PRIVATE ${WARNING_FLAGS})

    # Advanced example
    add_executable(advanced_example advanced_example.cpp)
    target_link_libraries(advanced_example PRIVATE cppcliargs::cppcliargs)
    target_compile_options(advanced_example PRIVATE ${WARNING_FLAGS})
endif()

# Tests
if(CPPCLIARGS_BUILD_TESTS)
    enable_testing()
    
    add_executable(test_cppcliargs test_cppcliargs.cpp)
    target_link_libraries(test_cppcliargs PRIVATE cppcliargs::cppcliargs)
    target_compile_options(test_cppcliargs PRIVATE ${WARNING_FLAGS})
    
    add_test(NAME cppcliargs_tests COMMAND test_cppcliargs)
endif()

# Installation
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS cppcliargs
    EXPORT cppcliargs-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES cppcliargs.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT cppcliargs-targets
    FILE cppcliargs-targets.cmake
    NAMESPACE cppcliargs::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppcliargs
)

# Generate config file (only if template exists)
set(CONFIG_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cppcliargs-config.cmake.in")
if(EXISTS "${CONFIG_TEMPLATE}")
    configure_package_config_file(
        ${CONFIG_TEMPLATE}
        ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppcliargs
    )
    
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppcliargs
    )
else()
    # Generate minimal config inline
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config.cmake
"# cppcliargs CMake configuration
include(\"\${CMAKE_CURRENT_LIST_DIR}/cppcliargs-targets.cmake\")
")
    
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppcliargs
    )
    
    message(STATUS "Note: cmake/cppcliargs-config.cmake.in not found, using generated config")
endif()

# Export from build tree
export(EXPORT cppcliargs-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cppcliargs-targets.cmake
    NAMESPACE cppcliargs::
)

# Print configuration summary
message(STATUS "")
message(STATUS "cppcliargs configuration:")
message(STATUS "  Version:         ${PROJECT_VERSION}")
message(STATUS "  Build examples:  ${CPPCLIARGS_BUILD_EXAMPLES}")
message(STATUS "  Build tests:     ${CPPCLIARGS_BUILD_TESTS}")
message(STATUS "  C++ standard:    C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
