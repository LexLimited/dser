#ifndef __DSER_HTTP_H__
#define __DSER_HTTP_H__

#include "dser/utils.h"
#include <string>
#include <unordered_map>

namespace dser::http {

    struct http_header_hash {
        /*
        size_t operator()(const std::string &sv) {
            return std::hash<std::string>{}(dser::to_lower_case(sv));
        }
        */

        size_t operator()(const std::string &sv) const {
            return std::hash<std::string>{}(dser::to_lower_case(sv));
        }
    };

    struct http_header_equal_to {
        bool operator()(const std::string& lhs, const std::string& rhs) const {
            return dser::to_lower_case(lhs) == dser::to_lower_case(rhs);
        }
    };

    enum class http_protocol {
        HTTP,
        HTTPS,
        INVALID
    };

    enum class http_protocol_version {
        V_1_0,
        V_1_1,
        V_2_0,
        INVALID
    };

    enum class http_method {
        GET,
        PUT,
        POST,
        DELETE,
        UPDATE,
        INVALID
    };

    constexpr std::string_view http_protocol_to_str(http_protocol p) {
        switch (p) {
            case http_protocol::HTTP: return "HTTP";
            case http_protocol::HTTPS: return "HTTPS";
            default: return {};
        }
    }

    constexpr std::string_view http_protocol_version_to_str(http_protocol_version pv) {
        switch (pv) {
            case http_protocol_version::V_1_0: return "1.0";
            case http_protocol_version::V_1_1: return "1.1";
            case http_protocol_version::V_2_0: return "2.0";
            default: return {};
        }
    }

    constexpr std::string_view http_method_to_string(http_method m) {
        switch (m) {
            case http_method::GET: return "GET";
            case http_method::PUT: return "PUT";
            case http_method::POST: return "POST";
            case http_method::DELETE: return "DELETE";
            case http_method::UPDATE: return "UPDATE";
            default: return {};
        }
    }

    class http {
        public:
            typedef std::unordered_map<std::string, std::string, http_header_hash, http_header_equal_to> Headers; 

            http();
            ~http();
    
            int read_from_file(const char* path);
            std::string stringify() const;

            void set_status_code(int code) { this->_status_code = code; }
            void set_status(std::string_view status) { this->_status = status; }
            void set_body(const std::string_view body) { this->_body = body; }
            void set_protocol(http_protocol p) noexcept { this->_protocol = p; }
            void set_protocol_vertsion(http_protocol_version pv) noexcept { this->_protocol_version = pv; }
            void set_method(http_method method) noexcept { this->_method = method; }
            void set_url(std::string_view url) noexcept { this->_url = url; }

            void set_header(const std::string_view key, const std::string_view value);
            void remove_header(const std::string_view key);

            const std::string& status() const noexcept { return this->_status; }
            int status_code() const noexcept { return this->_status_code; }
            http_protocol protocol() const noexcept { return this->_protocol; }
            http_protocol_version protocol_version() const noexcept { return this->_protocol_version; }
            std::string url() { return this->_url; }
            http_method method() { return this->_method; }
            const std::string& body() const noexcept { return this->_body; }
            const Headers& headers() const noexcept { return this->_headers; }
            const std::string header(std::string_view key) const;

        private:
            int _status_code = 404;
            std::string _status = "";
            http_protocol _protocol = http_protocol::HTTP;
            http_protocol_version _protocol_version = http_protocol_version::V_1_1;
            std::string _url = "";
            http_method _method = http_method::GET;
            std::string _body = {};
            Headers _headers = {};
    };

}

#endif // __DSER_HTTP_H__

