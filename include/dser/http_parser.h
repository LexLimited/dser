#ifndef __DSER_HTTP_PARSER_H__
#define __DSER_HTTP_PARSER_H__

#include "http.h"

namespace dser::http {

    class http_parser {
        public:
            enum class http_parser_error {
                MALFORMED_HTTP,
                MALFORMED_STATUS_LINE,
                MALFORMED_HEADER,
                EMPTY_HEADER_KEY,
                EMPTY_HEADER_VALUE,
                HEADER_MISSING_VALUE,
                INVALID_PROTOCOL,
                INVALID_PROTOCOL_VERSION,
                INVALID_METHOD,
                NO_ERROR,
                UNKNOWN
            };

            static constexpr std::string_view strerror(http_parser_error e) {
                switch (e) {
                    case http_parser_error::MALFORMED_HTTP: return "Malformed http";
                    case http_parser_error::MALFORMED_STATUS_LINE: return "Malformed status line";
                    case http_parser_error::MALFORMED_HEADER: return "Malformed header";
                    case http_parser_error::EMPTY_HEADER_KEY: return "Empty header key";
                    case http_parser_error::EMPTY_HEADER_VALUE: return "Empty header value";
                    case http_parser_error::HEADER_MISSING_VALUE: return "Header missing value";
                    case http_parser_error::INVALID_PROTOCOL: return "Invalid protocol";
                    case http_parser_error::INVALID_PROTOCOL_VERSION: return "Invalid protocol version";
                    case http_parser_error::INVALID_METHOD: return "Invalid method";
                    case http_parser_error::NO_ERROR: return "No error";
                    default: return "UNKNOWN";
                }
            }

            http_parser_error parse_http_string(std::string& str, http& h);    
			http_protocol parse_protocol(std::string_view sv);
			http_protocol_version parse_protocol_version(std::string_view sv);
			http_method parse_http_method(std::string_view sv);
			http_parser_error parse_protocol_and_version(std::string_view sv, http& h);
			http_parser_error parse_status_line(std::string_view str, http& http);
			http_parser_error parse_headers(std::string_view str, http& http);
    
        private:
    };

}

#endif // __DSER_HTTP_PARSER_H__

