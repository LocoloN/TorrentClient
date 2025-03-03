cmake_minimum_required(VERSION 3.30)
project(TorrentClient)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_FILES 
src/main.cpp
src/bencodeParse/bencodeparse.cpp
src/bencodeParse/parser.cpp
)
set(TEST_FILES
test/test-main.cpp
test/bencodeParseTests.cpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_library(source_lib ${SOURCE_FILES})
target_include_directories(source_lib PRIVATE include/)

target_include_directories(TorrentClient PRIVATE include)

add_executable(tests ${TEST_FILES})


list(APPEND CMAKE_PREFIX_PATH "E:/C++ libs/Catch2")
find_package(Catch2 3 REQUIRED)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain source_lib)

include(CTest)
include(Catch)
catch_discover_tests(tests)

target_include_directories(tests PRIVATE "include")