#ifndef __DSER_HTTP_H__
#define __DSER_HTTP_H__

#include <string>
#include <unordered_map>

namespace dser::http {

    enum http_protocol {
        HTTP,
        HTTPS
    };

    enum http_protocol_version {
        V_1_0,
        V_1_1,
        V_2_0
    };

    enum http_method {
        GET,
        PUT,
        POST,
        DELETE,
        UPDATE
    };

    constexpr const char* http_protocol_to_str(http_protocol p) {
        if (p == http_protocol::HTTP) return "HTTP";
        if (p == http_protocol::HTTPS) return "HTTPS";
        return nullptr;
    }

    constexpr const char* http_protocol_version_to_str(http_protocol_version pv) {
        if (pv == http_protocol_version::V_1_0) return "1.0";
        if (pv == http_protocol_version::V_1_1) return "1.1";
        if (pv == http_protocol_version::V_2_0) return "2.0";
        return nullptr;
    }

    constexpr const char* http_method_to_string(http_method m) {
        if (m == http_method::GET) return "GET";
        if (m == http_method::PUT) return "PUT";
        if (m == http_method::POST) return "POST";
        if (m == http_method::DELETE) return "DELETE";
        if (m == http_method::UPDATE) return "UPDATE";
        return nullptr;
    }

    class http {
        public:
            typedef std::unordered_map<std::string, std::string> Headers; 

            http();
            ~http();
    
            int read_from_file(const char* path);
            std::string stringify() const;

            inline void set_status_code(int code) { this->_status_code = code; }
            inline void set_status(std::string status) { this->_status = status; }
            inline void set_body(const std::string& body) { this->_body = body; }
            
            void set_header(const std::string& key, const std::string& value);
            void remove_header(const std::string& key);

            inline const std::string& status() const noexcept { return this->_status; }
            inline int status_code() const noexcept { return this->_status_code; }
            inline http_protocol protocol() const noexcept { return this->_protocol; }
            inline http_protocol_version protocol_version() const noexcept { return this->_protocol_version; }
            inline std::string url() { return this->_url; }
            inline http_method method() { return this->_method; }
            inline const std::string& body() const noexcept { return this->_body; }
            inline const Headers& headers() const noexcept { return this->_headers; }

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

