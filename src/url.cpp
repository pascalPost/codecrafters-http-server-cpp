#include "../include/url.h"

#include <cassert>
#include <regex>
#include <vector>
#include <string>
#include <regex>
#include <ranges>

namespace http_server {
    [[nodiscard]] std::vector<std::string> parseBracedExpressions(const std::string_view url) {
        std::vector<std::string> result;
        const std::regex braced_expr(R"(\{([^\}]+)\})");
        std::match_results<std::string_view::const_iterator> match;
        auto searchStart{url.cbegin()};

        while (std::regex_search(searchStart, url.cend(), match, braced_expr)) {
            result.push_back(match[1].str());
            searchStart = match.suffix().first;
        }

        return result;
    }

    [[nodiscard]] std::string replaceBracedExpressionsWithRegex(const std::string_view url,
                                                                const std::vector<std::string> &expressions,
                                                                const std::string_view insert_regex =
                                                                        "([a-zA-Z0-9-_~.]+)") {
        std::string result = std::string{url};
        const std::regex braced_expr(R"(\{[^\}]+\})");

        for ([[maybe_unused]] const auto &expr: expressions) {
            result = std::regex_replace(result, braced_expr, std::string{insert_regex},
                                        std::regex_constants::format_first_only);
        }

        return result;
    }

    [[nodiscard]] std::pair<std::vector<std::string>, std::string> transformBracedExpressions(
        const std::string_view url, const std::string_view insert_regex = "([a-zA-Z0-9-_~.]+)") {
        std::vector<std::string> expressions = parseBracedExpressions(url);
        auto transformed_url = replaceBracedExpressionsWithRegex(url, expressions, insert_regex);
        return std::make_pair(std::move(expressions), std::move(transformed_url));
    }

    UrlPattern::UrlPattern(std::unordered_map<std::string, std::string> &&m): pattern_map_{std::move(m)} {
    }

    std::optional<std::string> UrlPattern::get(const std::string &pattern) const {
        std::optional<std::string> value{};

        try {
            value = pattern_map_.at(pattern);
        } catch (const std::out_of_range &) {
            return {};
        }

        return value;
    }

    bool operator==(const UrlPattern &lhs, const UrlPattern &rhs) {
        return lhs.pattern_map_ == rhs.pattern_map_;
    }

    Url::Url(const std::string_view url) {
        auto [expressions, transformed_url] = transformBracedExpressions(url);
        this->url = std::move(transformed_url);
        matchers = std::move(expressions);
    }

    std::optional<UrlPattern> Url::match(const std::string_view url) const {
        const std::string url_str{url};
        if (std::smatch match; std::regex_match(url_str, match, std::regex{this->url})) {
            std::unordered_map<std::string, std::string> pattern_mapping{};
            pattern_mapping.reserve(matchers.size());

            assert(match.size() == matchers.size() + 1);

            for (auto &&[i, m]: std::views::enumerate(matchers)) {
                pattern_mapping.try_emplace(m, match[static_cast<std::size_t>(i) + 1]);
            }

            return UrlPattern{std::move(pattern_mapping)};
        }

        return {};
    }

    bool operator==(const Url &lhs, const Url &rhs) {
        return lhs.data() == rhs.data();
    }
}
