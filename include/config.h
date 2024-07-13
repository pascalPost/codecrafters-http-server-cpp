#pragma once

#include <optional>
#include <string>

/// Config represents the runtime selectable options that could be set via arguments or, e.g., environment variables,
/// config files, etc.
///
/// Things that could be added:
/// - port
/// - address
class Config {
public:
    Config() = default;

    [[nodiscard]] const std::optional<std::string> &directory() const {
        return directory_;
    }

    void set_directory(std::string &&directory) {
        directory_ = std::move(directory);
    }

private:
    std::optional<std::string> directory_;
};

/// parse the arguments and return the config
///
/// This function is only a minmal implementation right now. No help is printed. No complex parameter handling is
/// porvided and the single parameter needed is hardcoded. A better design would allow to register different options
/// and would create and print a help message based on this. This minimal setup is a placeholder for such a better
/// treatment.
Config parse(int argc, const char *const argv[]);
