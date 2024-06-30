#include "../include/log.h"

namespace http_server {
    static auto GLOBAL_LEVEL = log_level::DEBUG;

    std::string_view to_string(const log_level level) {
        switch (level) {
            case log_level::NOTHING: return "";
            case log_level::CRITICAL: return "CRITICAL";
            case log_level::ERROR: return "ERROR";
            case log_level::WARNING: return "WARNING";
            case log_level::INFO: return "INFO";
            case log_level::DEBUG: return "DEBUG";
        }
        return "";
    }

    log_level global_log_level() {
        return GLOBAL_LEVEL;
    }

    void set_global_log_level(const log_level level) {
        GLOBAL_LEVEL = level;
    }
}
