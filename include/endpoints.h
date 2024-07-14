#pragma once

#include <functional>
#include <unordered_map>

#include "endpoint-input.h"
#include "http.h"
#include "url.h"

namespace http_server {
    /// Endpoints encapsulates the storage of and the runtime dispatch to the endpoints.
    class Endpoints {
    public:
        Endpoints() = default;

        void add(
            std::string_view method,
            std::string_view path,
            std::function<std::string(const Endpoint_input &)> &&f);

        std::optional<std::string> run(const http::messages::Method method,
                                       const std::string_view target,
                                       const http::messages::Request &request
        ) const {
            const auto &method_endpoints = endpoints_[get_index(method)];
            for (const auto &[url, func]: method_endpoints) {
                if (const auto &url_pattern_on_match = Url{url}.match(target); url_pattern_on_match) {
                    return func(Endpoint_input{request, *url_pattern_on_match});
                }
            }

            return {};
        }

    private:
        std::array<std::unordered_map<Url, std::function<std::string(
            const Endpoint_input &)> >, http::messages::method::count> endpoints_;
    };
}
