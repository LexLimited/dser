#include "dser/utils.h"
#include <dser/file.h>
#include <dser/http.h>
#include <format>
#include <string>

using namespace dser::http;

http::http() {}

http::~http() {}

int http::read_from_file(const char* path)
{

    return 0;
}

std::string http::stringify() const
{
    std::string_view protocol = http_protocol_to_str(this->_protocol);
    std::string_view protocol_version = http_protocol_version_to_str(this->_protocol_version);
    if (!protocol.size() || !protocol_version.size()) return {};

    std::string str;
    std::string status_line = std::format("{}/{} {} {}\r\n",
            protocol, protocol_version,
            std::to_string(this->_status_code), this->_status);

    str += status_line;

    for (const auto& [key, value] : this->_headers)
    {
        str += std::format("{}: {}\r\n", key, value);
    }
    
    if (this->_body.size())
    {
        str += std::format("Content-Length:{}\r\n\r\n", this->_body.size());
        str += this->_body + "\r\n\r\n";
    } else
    {
        str += "\r\n";
    }

    return str;
}

void http::set_header(const std::string_view key, const std::string_view value)
{
    this->_headers[std::string{key}] = value;
}

void http::remove_header(const std::string_view key)
{
    const auto it = this->_headers.find(std::string{key});
    if (it == this->_headers.end()) return;
    this->_headers.erase(it);
}

const std::string http::header(std::string_view key) const
{
    const auto& h_it = this->_headers.find(std::string{key});
    if (h_it == this->_headers.end()) return {};
    return (*h_it).second;
}

