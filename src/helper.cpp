#include "../include/helper.h"
#include <cstring>

namespace http_server {
std::string get_error_description() { return std::string{strerror(errno)}; }
} // namespace http_server