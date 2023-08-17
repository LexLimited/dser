#include "dser/http.h"
#include <dser/router.h>
#include <iostream>

namespace dser {

    void router::get(std::string_view pattern, handler_func h)
    {
        this->_path_matches[std::string(pattern)] = { h, http::http_method::GET };
    }

    void router::put(std::string_view pattern, handler_func h)
    {
        this->_path_matches[std::string(pattern)] = { h, http::http_method::PUT };
    }

    void router::post(std::string_view pattern, handler_func h)
    {
        this->_path_matches[std::string(pattern)] = { h, http::http_method::POST };
    }

    void router::route(std::string_view pattern, std::function<void(router&)> route_func)
    {
        this->_subrouters[std::string(pattern)] = router{};
        route_func((*this->_subrouters.find(std::string(pattern))).second);
    }

    void match(const http::http& http)
    {
        std::cout << http.url() << std::endl;
    }
}

