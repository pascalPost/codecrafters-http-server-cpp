[![progress-banner](https://backend.codecrafters.io/progress/http-server/b886c6d8-68c7-4ea4-9531-5e857c8479a2)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

This is a starting point for C++ solutions to the
["Build Your Own HTTP server" Challenge](https://app.codecrafters.io/courses/http-server/overview).

**Note**: If you're viewing this repo on GitHub, head over to
[codecrafters.io](https://codecrafters.io) to try the challenge.

# http-server-cpp

This repo contains my C++ style implementation of a http server to parse the codecrafters
challenge. The focus is not to pass the tests with minimal effort, but to provide a solution
in the spirit of C++ (at least my understanding of it :wink:).

The features are:

- allows to easily specify endpoints for the server:

```cpp
server.add_endpoint("/", [](const auto &) -> std::string { return "HTTP/1.1 200 OK\r\n\r\n"; });
```

- url patten matching: `/echo/{str}` will match `/echo/abc` and provide a mapping of `str`
  to `abc`, e.g.:

```cpp
 server.add_endpoint("/echo/{str}", [](const auto &matches) -> std::string {
     const std::string str = matches.at("str");
     return std::format("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: {}\r\n\r\n{}", str.size(),
                        str);
 });
```
