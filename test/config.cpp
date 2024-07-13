#include "catch2/catch_test_macros.hpp"

#include "../include/config.h"

#include "catch2/generators/catch_generators.hpp"

[[nodiscard]] std::vector<std::string> line_to_words(const std::string &line) {
    std::vector<std::string> args;

    std::istringstream iss{line};
    std::string s;
    while (getline(iss, s, ' ')) {
        args.emplace_back(s);
    }

    return args;
}

std::vector<const char *> c_str(const std::vector<std::string> &words) {
    std::vector<const char *> args{};
    args.reserve(words.size());
    for (const auto &word: words) {
        args.emplace_back(word.c_str());
    }
    return args;
}

TEST_CASE("environment arguments parsing") {
    SECTION("directory specified") {
        const std::string line{"--directory dir"};
        const auto &args = c_str(line_to_words("filename " + line));
        const Config config = parse(static_cast<int>(args.size()), args.data());
        REQUIRE(config.directory() == "dir");
    }

    SECTION("missing argument to directory") {
        const std::string line{"--directory"};
        const auto &args = c_str(line_to_words("filename " + line));
        REQUIRE_THROWS(parse(static_cast<int>(args.size()), args.data()));
    }

    SECTION("no directory specified") {
        const std::string line{"--dir"};
        const auto &args = c_str(line_to_words("filename " + line));
        const Config config = parse(static_cast<int>(args.size()), args.data());
        REQUIRE_FALSE(config.directory());
    }
}
