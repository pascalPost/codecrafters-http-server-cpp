#include "catch2/catch_test_macros.hpp"

#include <regex>
#include <vector>
#include <string>
#include <unordered_map>
#include <ranges>

[[nodiscard]] std::vector<std::string> parseBracedExpressions(const std::string &url) {
    std::vector<std::string> result;
    std::regex braced_expr(R"(\{([^\}]+)\})");
    std::smatch match;
    std::string::const_iterator searchStart(url.cbegin());

    while (std::regex_search(searchStart, url.cend(), match, braced_expr)) {
        result.push_back(match[1].str());
        searchStart = match.suffix().first;
    }

    return result;
}

[[nodiscard]] std::string replaceBracedExpressionsWithRegex(const std::string &url,
                                                            const std::vector<std::string> &expressions,
                                                            std::string_view insert_regex = "([a-zA-Z0-9-_~.]+)") {
    std::string result = url;
    std::regex braced_expr(R"(\{[^\}]+\})");

    for (const auto &expr: expressions) {
        result = std::regex_replace(result, braced_expr, std::string{insert_regex},
                                    std::regex_constants::format_first_only);
    }

    return result;
}

[[nodiscard]] std::pair<std::vector<std::string>, std::string> transformBracedExpressions(
    const std::string &url, std::string_view insert_regex = "([a-zA-Z0-9-_~.]+)") {
    std::vector<std::string> expressions = parseBracedExpressions(url);
    auto transformed_url = replaceBracedExpressionsWithRegex(url, expressions, insert_regex);
    return std::make_pair(std::move(expressions), std::move(transformed_url));
}

TEST_CASE("url pattern") {
    {
        REQUIRE(transformBracedExpressions("/echo/{test}/reg/{abc}") == std::pair<std::vector<std::string>,
                std::string>{{"test", "abc"}, "/echo/([a-zA-Z0-9-_~.]+)/reg/([a-zA-Z0-9-_~.]+)"});
    }
}

class Url {
public:
    using MatchT = std::optional<std::unordered_map<std::string, std::string> >;

    explicit Url(const std::string &url) {
        auto [expressions, transformed_url] = transformBracedExpressions(url);
        this->url = std::move(transformed_url);
        matchers = std::move(expressions);
    }

    [[nodiscard]] MatchT match(const std::string &url) const {
        if (std::smatch match; std::regex_match(url, match, std::regex{url})) {
            std::unordered_map<std::string, std::string> matched{};
            matched.reserve(matchers.size());

            assert(match.size() == matchers.size() + 1);

            for (const auto [i, m]: std::views::enumerate(matchers)) {
                matched.try_emplace("m", match[i + 1]);
            }

            return matched;
        }

        return {};
    }

private:
    std::string url;
    std::vector<std::string> matchers;
};

std::string parseURL(const std::string &url) {
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

TEST_CASE("url matching") {
    const Url url{"/echo/{param}"}; {
        using ResT = Url::MatchT::value_type;
        REQUIRE(url.match("/echo/hello") == ResT{{"param", "hello"}});
        REQUIRE(url.match("/echo/world") == ResT{{"param", "world"}});
        REQUIRE(url.match("/echo/special-characters_~.") == ResT{{"param", "special-characters_~."}});
        REQUIRE(url.match("/echo/hello/world") == std::nullopt);
        REQUIRE(url.match("/echo/hello/") == std::nullopt);
    }
}
