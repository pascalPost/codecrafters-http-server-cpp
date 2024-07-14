#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "http.h"


namespace http_server::http::messages {
    namespace method {
        using enum_type = u_int8_t;

        static constexpr int count{9};
    }

    enum class Method : method::enum_type {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH
    };

    [[nodiscard]] Method from_string(std::string_view method);

    [[nodiscard]] method::enum_type get_index(Method method) noexcept;

    class Request_line {
    public:
        explicit Request_line(std::string &&data);

        [[nodiscard]] Method http_method() const;

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
