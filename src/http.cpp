#include "../include/http.h"

#include <ranges>
#include <vector>

namespace http_server::http::messages {
    Request_line::Request_line(std::string &&line): data{std::move(line)} {
        std::vector<std::string_view> split;
        split.reserve(3);
        for (const auto word: std::views::split(data, std::string_view(" "))) {
            split.emplace_back(word);
        }

        if (split.size() != 3) {
            throw std::runtime_error("Invalid HTTP request line.");
        }

        http_method_view = split[0];
        request_target_view = split[1];
        http_version_view = split[2];
    }

    std::string Request_line::http_method() const {
        return std::string{http_method_view};
    }

    std::string Request_line::request_target() const {
        return std::string{request_target_view};
    }

    std::string Request_line::http_version() const {
        return std::string{http_version_view};
    }

    std::string_view Request_line::line() const {
        return data;
    }


    Request_line Request::request_line() const {
        return Request_line{std::string{request_line_view}};
    }

    std::string_view Request::headers() const {
        return headers_view;
    }

    std::string_view Request::body() const {
        return body_view;
    }

    Request::Request(std::string &&m): message{m} {
        const size_t pos_request_line_end = message.find("\r\n");
        if (pos_request_line_end == std::string::npos) {
            throw std::runtime_error("Invalid HTTP message: no request line separator found");
        }

        const size_t pos_headers_end = message.find("\r\n\r\n");
        if (pos_headers_end == std::string::npos) {
            throw std::runtime_error("Invalid HTTP message: no header-body separator found");
        }

        request_line_view = std::string_view{message}.substr(0, pos_request_line_end);
        headers_view = std::string_view{message}.substr(pos_request_line_end + 2,
                                                        pos_headers_end - (pos_request_line_end + 2));
        body_view = std::string_view{message}.substr(pos_headers_end + 4);
    }
}
