#pragma once

#include <stdexcept>
#include <string>
#include <string_view>


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


    class Request {
    public:
        explicit Request(std::string &&m);

        [[nodiscard]] Request_line request_line() const;

        [[nodiscard]] std::string_view headers() const;

        [[nodiscard]] std::string_view body() const;

    private:
        std::string message;
        std::string_view request_line_view;
        std::string_view headers_view;
        std::string_view body_view;
    };
}
