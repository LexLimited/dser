#ifndef __DSER_ROUTER_H__
#define __DSER_ROUTER_H__

#include <string>
#include <functional>
#include <unordered_map>

#include "inet_socket.h"
#include "http.h"

namespace dser {

    using handler_func = std::function<void(inet_socket&, http::http&)>;
    
    struct path_matching_item {
        handler_func handler;
        http::http_method method;
    };

    class router {
        public:

            void get(std::string_view, handler_func);
            void put(std::string_view, handler_func);
            void post(std::string_view, handler_func);
            void route(std::string_view, std::function<void(router&)>);

            void match(const http::http& http);

            const std::unordered_map<std::string, path_matching_item> &path_matches() const noexcept { return this->_path_matches; };
            const std::unordered_map<std::string, router> &subrouters() const noexcept { return this->_subrouters; };

        private:
            std::unordered_map<std::string, path_matching_item> _path_matches;
            std::unordered_map<std::string, router> _subrouters;
    };

}

#endif // __DSER_ROUTER_H__

