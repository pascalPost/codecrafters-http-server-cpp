#include "catch2/catch_test_macros.hpp"

#include <regex>

std::string parseURL(const std::string& url) {
    // Define the regex pattern. The pattern \{str\} will be replaced by (.+) to capture any string
    std::regex pattern("/echo/([a-zA-Z0-9-_~.]+)");
    std::smatch match;

    // Perform the regex match
    if (std::regex_match(url, match, pattern) && match.size() == 2) {
        // Extract the matched part (excluding the whole match at index 0)
        return match[1].str();
    } else {
        // Return an empty string if the match fails
        return "";
    }
}

TEST_CASE("url routing") {
    REQUIRE(parseURL("/echo/hello") == "hello");
    REQUIRE(parseURL("/echo/world") == "world");
    REQUIRE(parseURL("/echo/special-characters_~.") == "special-characters_~.");
    REQUIRE(parseURL("/echo/hello/world") == "");
    REQUIRE(parseURL("/echo/hello/") == "");
}