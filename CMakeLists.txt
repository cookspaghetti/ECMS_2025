cmake_minimum_required(VERSION 3.10)

# Project name and version
project(EsportsChampionshipManagamentSystem 
        VERSION 1.0.0
        DESCRIPTION "Esports Championship Management System"
        LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Build options
option(BUILD_TESTS "Build test programs" OFF)
option(BUILD_DOCS "Build documentation" OFF)

# Include header files
include_directories(include)

# Main application source files 
file(GLOB_RECURSE APP_SOURCES "src/*.cpp")
list(FILTER APP_SOURCES EXCLUDE REGEX "src/tests/.*\\.cpp$")

# Executable output
add_executable(EsportsChampionshipManagamentSystem ${APP_SOURCES})

# Set compiler warnings
if(MSVC)
    target_compile_options(EsportsChampionshipManagamentSystem PRIVATE /W4)
else()
    target_compile_options(EsportsChampionshipManagamentSystem PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Testing configuration
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation rules
install(TARGETS EsportsChampionshipManagamentSystem
        RUNTIME DESTINATION bin)
install(DIRECTORY include/ 
        DESTINATION include)

# Generate documentation if requested
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, documentation will not be built")
    endif()
endif()

# Print summary
message(STATUS "")
message(STATUS "EsportsChampionshipManagamentSystem configuration summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build docs: ${BUILD_DOCS}")
message(STATUS "  Sources being compiled:")
foreach(source_file ${APP_SOURCES})
    message(STATUS "  ${source_file}")
endforeach()

message(STATUS "")
