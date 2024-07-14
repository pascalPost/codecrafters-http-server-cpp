#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace http_server {
    class UrlPattern {
    public:
        explicit UrlPattern(std::unordered_map<std::string, std::string> &&);

        [[nodiscard]] std::optional<std::string> get(const std::string &pattern) const;

    private:
        std::unordered_map<std::string, std::string> pattern_map_;

        friend bool operator==(const UrlPattern &, const UrlPattern &);
    };

    bool operator==(const UrlPattern &lhs, const UrlPattern &rhs);

    class Url {
    public:
        explicit Url(std::string_view url);

        [[nodiscard]] std::optional<UrlPattern> match(std::string_view url) const;

        [[nodiscard]] std::string data() const noexcept { return url; }

    private:
        std::string url;
        std::vector<std::string> matchers;
    };

    bool operator==(const Url &, const Url &);
}

template<>
struct std::hash<http_server::Url> {
    std::size_t operator()(http_server::Url const &u) const noexcept {
        return std::hash<std::string>{}(u.data());
    }
};
