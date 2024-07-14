#include "catch2/catch_test_macros.hpp"

#include <regex>
#include <vector>
#include <string>

#include "../include/url.h"

namespace http_server {
    extern std::pair<std::vector<std::string>, std::string> transformBracedExpressions(
        std::string_view url, std::string_view insert_regex = "([a-zA-Z0-9-_~.]+)");
}

TEST_CASE("url pattern detection") {
    REQUIRE(http_server::transformBracedExpressions("/echo/{test}/reg/{abc}") == std::pair<std::vector<std::string>,
            std::string>{{"test", "abc"}, "/echo/([a-zA-Z0-9-_~.]+)/reg/([a-zA-Z0-9-_~.]+)"});
}

TEST_CASE("url pattern matching") {
    const http_server::Url url{"/echo/{param}"};
    REQUIRE(url.match("/echo/hello").value().get("param") == "hello");
    REQUIRE(url.match("/echo/world").value().get("param") == "world");
    REQUIRE(url.match("/echo/special-characters_~.").value().get("param") == "special-characters_~.");
    REQUIRE_FALSE(url.match("/echo/hello/world").has_value());
    REQUIRE_FALSE(url.match("/echo/hello/").has_value());
}
