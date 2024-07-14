#include "../include/endpoints.h"

void http_server::Endpoints::add(const std::string_view method, const std::string_view path,
                                 std::function<std::string(const Endpoint_input &)> &&f) {
    using http::messages::from_string;
    using http::messages::get_index;
    const auto i = get_index(from_string(method));
    endpoints_[i].try_emplace(Url{path}, f);
}
