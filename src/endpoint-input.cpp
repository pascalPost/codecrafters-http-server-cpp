#include "../include/endpoint-input.h"

namespace http_server {
    Endpoint_input::Endpoint_input(const http::messages::Request &request,
                                   const UrlPattern &url_pattern): request_(request),
                                                                   url_pattern_(url_pattern) {
    }

    const http::messages::Request &Endpoint_input::request() const {
        return request_;
    }

    const UrlPattern &Endpoint_input::url_pattern() const {
        return url_pattern_;
    }
}
