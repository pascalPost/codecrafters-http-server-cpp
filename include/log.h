#pragma once

#include <print>
#include <iostream>

namespace http_server {
    enum class log_level : unsigned char {
        NOTHING = 0,
        CRITICAL,
        ERROR,
        WARNING,
        INFO,
        DEBUG
    };


    std::string_view to_string(log_level);

    log_level global_log_level();

    void set_global_log_level(log_level);

    template<log_level level, class... Args>
    void log(std::format_string<Args...> fmt, Args &&... args) {
        if (static_cast<unsigned char>(level) <= static_cast<unsigned char>(global_log_level())) {
            auto message = std::format(fmt, std::forward<Args>(args)...);
            std::println("[{}] : {}", to_string(level), message);
        }
    }
}
