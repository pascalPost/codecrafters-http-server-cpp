#include "catch2/catch_test_macros.hpp"

#include <regex>
#include <vector>
#include <string>
#include <unordered_map>

#include "../include/url.h"

namespace http_server {
    extern std::pair<std::vector<std::string>, std::string> transformBracedExpressions(
        const std::string &url, std::string_view insert_regex = "([a-zA-Z0-9-_~.]+)");
}

TEST_CASE("url pattern detection") {
    REQUIRE(http_server::transformBracedExpressions("/echo/{test}/reg/{abc}") == std::pair<std::vector<std::string>,
            std::string>{{"test", "abc"}, "/echo/([a-zA-Z0-9-_~.]+)/reg/([a-zA-Z0-9-_~.]+)"});
}

TEST_CASE("url pattern matching") {
    const http_server::Url url{"/echo/{param}"};
    using ResT = http_server::Url::MatchT::value_type;
    REQUIRE(url.match("/echo/hello") == ResT{{"param", "hello"}});
    REQUIRE(url.match("/echo/world") == ResT{{"param", "world"}});
    REQUIRE(url.match("/echo/special-characters_~.") == ResT{{"param", "special-characters_~."}});
    REQUIRE(url.match("/echo/hello/world") == std::nullopt);
    REQUIRE(url.match("/echo/hello/") == std::nullopt);
}
