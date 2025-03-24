cmake_minimum_required(VERSION 3.30)
project(TorrentClient)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_CODE_FILES 
src/filesystem.cpp
src/torrentFiles/torrentFileInfo.cpp
src/torrentFiles/lazyTorrentFile.cpp
src/bencodeParse/bencodeparse.cpp
src/bencodeParse/parser.cpp
)

set(TEST_FILES
test/test-main.cpp
test/bencodeParseTests.cpp
test/filesystemTests.cpp
test/lazyTorrentTest.cpp
)

add_library(source_lib ${SOURCE_CODE_FILES})
target_include_directories(source_lib PUBLIC include/ src/)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE source_lib)
target_include_directories(${PROJECT_NAME} PRIVATE include/)

add_executable(tests ${TEST_FILES})
target_link_libraries(tests PRIVATE source_lib)
target_include_directories(tests PRIVATE include/)

find_package(Catch2 3 REQUIRED)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
include(CTest)
include(Catch)
catch_discover_tests(tests)
