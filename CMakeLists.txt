cmake_minimum_required(VERSION 3.16)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

project(HTTPResumer VERSION 0.1.0 LANGUAGES CXX)
# Use the version in project settings
set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Add project name as a variable
set(PROJECT_NAME ${CMAKE_PROJECT_NAME})

# Set the value for BAR_WIDTH
set(BAR_WIDTH 50)

# Set the value for BAR_CHAR
set(BAR_CHAR '=')

# Set the value for BAR_STR
set(BAR_STR "█")

# Set the value for BAR_EMPTY_STR
set(BAR_EMPTY_STR "░")

# Debugging output
message("PROJECT_NAME: ${PROJECT_NAME}")
message("VERSION: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Add version.h as a configured file to automatically set the version number
configure_file(
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
  @ONLY
)

# Add config.h 
configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  @ONLY
)

# Find dependencies
find_package(CURL REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


# Optionally, gather all source files in the project
file(GLOB_RECURSE ALL_SOURCE_FILES
    "*.cpp"
    "*.h"
)

set (HEADERS Downloader.h utils.h)
set (SOURCES main.cpp Downloader.cpp utils.cpp)

add_executable(httpresumer ${HEADERS} ${SOURCES})
target_include_directories(httpresumer PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(httpresumer CURL::libcurl)

# Testing
enable_testing() # Enable testing framework

add_subdirectory(tests)

# Conditionally define _WIN32
if(WIN32)
    target_compile_definitions(signal_handling PRIVATE _WIN32)
endif()
