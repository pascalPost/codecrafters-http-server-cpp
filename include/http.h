#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>


namespace http_server::http::messages {
    class Request_line {
    public:
        explicit Request_line(std::string &&data);

        [[nodiscard]] std::string http_method() const;

        [[nodiscard]] std::string request_target() const;

        [[nodiscard]] std::string http_version() const;

        [[nodiscard]] std::string_view line() const;

    private:
        std::string data;
        std::string_view http_method_view;
        std::string_view request_target_view;
        std::string_view http_version_view;
    };

    class Headers {
    public:
        explicit Headers(std::string_view content);

        [[nodiscard]] std::optional<std::string> field_value(const std::string &field_name);

        [[nodiscard]] std::string content() { return content_; }

    private:
        std::string content_;
        std::unordered_map<std::string, std::string> data_;
    };


    class Request {
    public:
        explicit Request(std::string &&m);

        [[nodiscard]] Request_line request_line() const;

        [[nodiscard]] Headers headers() const;

        [[nodiscard]] std::string_view body() const;

    private:
        std::string message;
        std::string_view request_line_view;
        std::string_view headers_view;
        std::string_view body_view;
    };
}
