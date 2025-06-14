cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Set the build type if not already defined
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Conan 2.x + CMake integration requires setting the toolchain before project()
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/build/conan_toolchain.cmake")
    message(FATAL_ERROR "The file conan_toolchain.cmake doesn't exist, you must install dependencies first from the project root directory:\nconan install . --output-folder=build --build=missing")
endif()
include("${CMAKE_SOURCE_DIR}/build/conan_toolchain.cmake")

project(wadconvert 
    VERSION 0.1.0
    DESCRIPTION "A WAD file converter"
    LANGUAGES CXX)

# Enable clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY 
        ${CLANG_TIDY_EXE};
        -extra-arg=-Wno-unknown-warning-option;
        -p=${CMAKE_BINARY_DIR}
    )
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
else()
    message(WARNING "clang-tidy not found!")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Always include debug info
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Generate compile_commands.json for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find dependencies (Conan 2.x style)
find_package(nlohmann_json REQUIRED)

# Source files for wadconvert
file(GLOB_RECURSE PROJECT_SOURCES 
    src/*.cpp
    src/*/*.cpp
    src/*.hpp
    src/*/*.hpp)

# Create main executable target
add_executable(wadconvert ${PROJECT_SOURCES})

target_include_directories(wadconvert
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Link libraries to the executable target
# Only nlohmann_json is needed

target_link_libraries(wadconvert
    PRIVATE
        nlohmann_json::nlohmann_json
)

if(APPLE)
    target_link_libraries(wadconvert
        PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
endif()

# Enable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(wadconvert PRIVATE
        -Wall 
        -Wextra 
        # -Wpedantic
        -Wundef                      # Warn on undefined macro usage
        # -Wreserved-macro-identifier  # Warn on reserved macro names
        -Wmacro-redefined            # Warn on macro redefinition
        -Wextra-semi                 # Warn on redundant semicolons
    )    
endif()