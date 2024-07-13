#include "../include/config.h"

#include <stdexcept>

Config parse(const int argc, const char *const argv[]) {
    Config config{};

    for (int i = 1; i < argc; ++i) {
        if (std::string arg{argv[i]}; arg == "--directory") {
            if (argc > i + 1) {
                std::string directory{argv[i + 1]};
                config.set_directory(std::move(directory));
            } else {
                throw std::runtime_error("--directory option without argument");
            }
        }
    }

    return config;
}
