#ifndef __DSER_ROUTER_H__
#define __DSER_ROUTER_H__

#include <string>
#include <functional>
#include <unordered_map>

#include "inet_socket.h"
#include "http.h"

namespace dser
{

    using handler_func = std::function<void(inet_socket&, http::http&)>;
    

    class router {
        /// An item implements a map pattern -> action.
        /// If `r == nullptr`, call the handler
        /// otherwise match http with `r`
        struct item
        {
            handler_func handler;
            std::string pattern;
            http::http_method method;
            router* r;
        };
    
        public:
            ~router();

            void get(std::string_view, handler_func);
            void put(std::string_view, handler_func);
            void post(std::string_view, handler_func);

            void route(std::string_view, std::function<void(router&)>);

            void match(const http::http& http);
        
        private:
            std::vector<item> _items;
    };

}

#endif // __DSER_ROUTER_H__

