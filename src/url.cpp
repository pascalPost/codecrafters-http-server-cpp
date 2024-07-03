#include "../include/url.h"

#include <cassert>
#include <regex>
#include <vector>
#include <string>
#include <regex>
#include <ranges>

namespace http_server {
    [[nodiscard]] std::vector<std::string> parseBracedExpressions(const std::string &url) {
        std::vector<std::string> result;
        const std::regex braced_expr(R"(\{([^\}]+)\})");
        std::smatch match;
        auto searchStart(url.cbegin());

        while (std::regex_search(searchStart, url.cend(), match, braced_expr)) {
            result.push_back(match[1].str());
            searchStart = match.suffix().first;
        }

        return result;
    }

    [[nodiscard]] std::string replaceBracedExpressionsWithRegex(const std::string &url,
                                                                const std::vector<std::string> &expressions,
                                                                const std::string_view insert_regex =
                                                                        "([a-zA-Z0-9-_~.]+)") {
        std::string result = url;
        const std::regex braced_expr(R"(\{[^\}]+\})");

        for ([[maybe_unused]] const auto &expr: expressions) {
            result = std::regex_replace(result, braced_expr, std::string{insert_regex},
                                        std::regex_constants::format_first_only);
        }

        return result;
    }

    [[nodiscard]] std::pair<std::vector<std::string>, std::string> transformBracedExpressions(
        const std::string &url, const std::string_view insert_regex = "([a-zA-Z0-9-_~.]+)") {
        std::vector<std::string> expressions = parseBracedExpressions(url);
        auto transformed_url = replaceBracedExpressionsWithRegex(url, expressions, insert_regex);
        return std::make_pair(std::move(expressions), std::move(transformed_url));
    }

    Url::Url(const std::string &url) {
        auto [expressions, transformed_url] = transformBracedExpressions(url);
        this->url = std::move(transformed_url);
        matchers = std::move(expressions);
    }

    Url::MatchT Url::match(const std::string &url) const {
        if (std::smatch match; std::regex_match(url, match, std::regex{this->url})) {
            std::unordered_map<std::string, std::string> matched{};
            matched.reserve(matchers.size());

            assert(match.size() == matchers.size() + 1);

            for (auto &&[i, m]: std::views::enumerate(matchers)) {
                matched.try_emplace(m, match[static_cast<std::size_t>(i) + 1]);
            }

            return matched;
        }

        return {};
    }

    bool operator==(const Url &lhs, const Url &rhs) {
        return lhs.data() == rhs.data();
    }
}
