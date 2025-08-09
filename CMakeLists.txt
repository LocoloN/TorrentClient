cmake_minimum_required(VERSION 3.30)
project(TorrentClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE SOURCE_CODE_FILES 
    "src/*.cpp")
list(REMOVE_ITEM SOURCE_CODE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

file(GLOB_RECURSE TEST_FILES "test/*.cpp")
list(FILTER TEST_FILES EXCLUDE REGEX "/testfiles/")

find_package(Catch2 3 REQUIRED)

add_library(source_lib ${SOURCE_CODE_FILES})
target_include_directories(source_lib PUBLIC "include/" "src/")

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE source_lib)
target_include_directories(${PROJECT_NAME} PRIVATE include/)

add_executable(tests ${TEST_FILES})
target_link_libraries(tests PRIVATE source_lib)
target_include_directories(tests PRIVATE "include/" "src/")

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
include(CTest)
include(Catch)
catch_discover_tests(tests)
