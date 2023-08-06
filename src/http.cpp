#include <dser/file.h>
#include <dser/http.h>
#include <format>
#include <string>

using namespace dser::http;

http::http() {}

http::~http() {}

int http::read_from_file(const char* path) {

    return 0;
}

std::string http::stringify() const {
    const char* protocol = http_protocol_to_str(this->_protocol);
    const char* protocol_version = http_protocol_version_to_str(this->_protocol_version);
    if (!protocol || !protocol_version) return {};

    std::string str;
    std::string status_line = std::format("{}/{} {} {}\n",
            protocol, protocol_version,
            std::to_string(this->_status_code), this->_status);

    str += status_line;

    for (const auto& [key, value] : this->_headers) {
        str += std::format("{}:{}\n", key, value);
    }
    
    if (this->_body.size()) {
        str += std::format("Content-Length:{}\n\n", this->_body.size());
        str += this->_body + "\n\n";
    }

    return str;
}

void http::set_header(const std::string& key, const std::string& value) {
    this->_headers[key] = value;
}

void http::remove_header(const std::string& key) {
    const auto it = this->_headers.find(key);
    if (it == this->_headers.end()) return;
    this->_headers.erase(it);
}

