#ifndef __DSER_HTTP_PARSER_H__
#define __DSER_HTTP_PARSER_H__

#include "http.h"

namespace dser {

    class http_parser {
        public:
            int parse_http_string(std::string& str, http::http& h);    
    
        private:
    };

}

#endif // __DSER_HTTP_PARSER_H__

