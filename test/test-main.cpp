#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
TEST_CASE("Main test example", "[example]") {
    REQUIRE(4+5 == 9);
}