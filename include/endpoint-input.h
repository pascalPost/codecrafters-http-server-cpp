#pragma once

namespace http_server {
    class UrlPattern;

    namespace http::messages {
        class Request;
        class Headers;
    }

    class Endpoint_input {
    public:
        Endpoint_input(const http::messages::Request &request, const UrlPattern &url_pattern);

        [[nodiscard]] const http::messages::Request &request() const;

        [[nodiscard]] const UrlPattern &url_pattern() const;

    private:
        const http::messages::Request &request_;
        const UrlPattern &url_pattern_;
    };
}
