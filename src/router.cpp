#include <iostream>

#include <dser/http.h>
#include <dser/router.h>
#include <dser/path.h>

namespace dser {

    /// As soon as wildcald is met, the rest is ignored
    static bool url_matches_pattern(const std::string_view& url, const std::string_view& pattern)
    {
        auto pattern_path = path(pattern);
        auto url_path = path(url);
        for (size_t i = 0; i < pattern_path.n_segments(); ++i)
        {
            std::cout << "Mathing " << pattern_path.get_segment(i) << std::endl;
            if (url_path.n_segments() < i)
                return true;
            if (pattern_path.get_segment(i) == "*")
                return true;
            if (url_path.get_segment(i) != pattern_path.get_segment(i))
                return false;
        }
        
        return true;
    }

    // public
    router::~router()
    {
        for (auto& item : this->_items)
        {
            if (item.router) delete item.router;
        }
    }

    // public
    void router::get(std::string_view pattern, handler_func h)
    {
        this->add_item(pattern, http::http_method::GET, h);
    }

    void router::put(std::string_view pattern, handler_func h)
    {
        this->add_item(pattern, http::http_method::PUT, h);
    }

    void router::post(std::string_view pattern, handler_func h)
    {
        this->add_item(pattern, http::http_method::POST, h);
    }

    void router::route(std::string_view pattern, std::function<void(router&)> route_func)
    {
        router* router = new class router();
        route_func(*router);
        this->add_item(pattern, router);
    }

    void router::match(inet_socket& sock, const http::http& http)
    {
        for (const auto& item : this->_items)
        {
            if (url_matches_pattern(http.url(), item.pattern))
            {
                if (item.router)
                    item.router->match(sock, http);
                else
                    item.handler(sock, http);
                return;
            }
        }
    }

    // private
    void router::add_item(
            const std::string_view& pattern,
            router* router)
    {
        auto it = item{
            .handler = nullptr,
            .pattern = std::string(pattern),
            .method = http::http_method::INVALID,
            .router = router};

        this->_items.push_back(it);
    }
    
    void router::add_item(
            const std::string_view& pattern,
            http::http_method method,
            handler_func handler)
    {
        auto it = item{
            .handler = handler,
            .pattern = std::string(pattern),
            .method = method,
            .router = nullptr};

        this->_items.push_back(it);
    }
}

