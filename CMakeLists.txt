cmake_minimum_required(VERSION 3.13)

project(http-server-starter-cpp)

set(SOURCE_FILES
        src/main.cpp
        src/server.cpp
        src/socket.cpp
        src/helper.cpp
        src/log.cpp
        src/http.cpp
)

set(CMAKE_CXX_STANDARD 23)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

add_library(httpCom ${SOURCE_FILES})
target_link_libraries(httpCom PRIVATE Threads::Threads ZLIB::ZLIB)
target_compile_options(httpCom PRIVATE -Werror -pedantic-errors -Wall -Wextra -Wconversion -Wsign-conversion)

add_executable(server src/main.cpp)
target_link_libraries(server httpCom)
target_compile_options(server PRIVATE -Werror -pedantic-errors -Wall -Wextra -Wconversion -Wsign-conversion)

# testing

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests
        test/test.cpp
        test/http.cpp
        test/routing.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain httpCom)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)