# Tests/CMakeLists.txt
# Google Test library
add_executable(tests
    test_downloader.cpp  # Your test source files
    test_utils.cpp
    # ${CMAKE_SOURCE_DIR}/build/CMakeFiles/HTTPResumer.dir/utils.cpp.o  # Link against utils.cpp.o
    ${CMAKE_SOURCE_DIR}/utils.cpp
    # ${ALL_SOURCE_FILES}  # Include all sources from the entire project

)

target_link_libraries(tests
    GTest::GTest
    GTest::Main
    ${CMAKE_THREAD_LIBS_INIT}  # If threading is used in tests
    CURL::libcurl             # Link with libcurl if needed in tests
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}  # Include the source directory for testing
    ${GTEST_INCLUDE_DIRS}  # Include directories for GTest
    ${CURL_INCLUDE_DIRS}   # Include directories for libcurl
    ${CMAKE_BINARY_DIR}  # Include build directory for HTTPResumer
)

# Add test target using CTest
include(GoogleTest)
gtest_discover_tests(tests)