#include "../include/http.h"

#include <ranges>
#include <vector>
#include <string_view>

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

    namespace {
        [[nodiscard]] std::string_view trim(const std::string_view str) {
            const size_t first = str.find_first_not_of(" \t\n\r\f\v");
            if (first == std::string::npos)
                return ""; // str is all whitespace

            const size_t last = str.find_last_not_of(" \t\n\r\f\v");
            return str.substr(first, (last - first + 1));
        }

        std::pair<std::string, std::string> split_field_into_name_and_value(const std::string_view field) {
            const size_t pos_field_split = field.find(':');
            if (pos_field_split == std::string::npos) {
                throw std::runtime_error("Invalid HTTP headers field: no field name value separator (:) found.");
            }

            if (pos_field_split == 1) {
                throw std::runtime_error("Invalid HTTP headers field: must not start with (:).");
            }

            std::string name{trim(field.substr(0, pos_field_split))};
            std::string value{trim(field.substr(pos_field_split + 1))};

            return std::make_pair(std::move(name), std::move(value));
        }

        [[nodiscard]] auto split_into_fields(const std::string_view headers) {
            std::unordered_map<std::string, std::string> headers_field_data{};
            for (const auto field: std::views::split(headers, std::string_view{"\r\n"})) {
                auto [name, value] = split_field_into_name_and_value(std::string_view{field});
                headers_field_data.try_emplace(std::move(name), std::move(value));
            }
            return headers_field_data;
        }
    }


    Headers::Headers(const std::string_view content): content_{content}, data_{split_into_fields(content)} {
    }

    std::optional<std::string> Headers::field_value(const std::string &field_name) {
        try {
            return data_.at(field_name);
        } catch (std::out_of_range &) {
            return {};
        }
    }


    Request_line Request::request_line() const {
        return Request_line{std::string{request_line_view}};
    }

    Headers Request::headers() const {
        return Headers{headers_view};
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
