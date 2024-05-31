#include "dser/http.h"
#include <dser/router.h>
#include <iostream>

namespace dser {

    void router::get(std::string_view pattern, handler_func h)
    {
        (void)pattern;
        (void)h;
    }

    void router::put(std::string_view pattern, handler_func h)
    {
        (void)pattern;
        (void)h;
    }

    void router::post(std::string_view pattern, handler_func h)
    {
        (void)pattern;
        (void)h;
    }

    void router::route(std::string_view pattern, std::function<void(router&)> route_func)
    {
        (void)pattern;
        (void)route_func;
    }

    void match(const http::http& http)
    {
        std::cout << http.url() << std::endl;
    }
}

