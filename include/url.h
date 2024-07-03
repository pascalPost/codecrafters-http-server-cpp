#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace http_server {
    class Url {
    public:
        using MatchT = std::optional<std::unordered_map<std::string, std::string> >;

        explicit Url(const std::string &url);

        [[nodiscard]] MatchT match(const std::string &url) const;

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
