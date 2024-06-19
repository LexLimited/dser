#ifndef __DSER_SOCKET_UTILS_H__
#define __DSER_SOCKET_UTILS_H__

#include <netdb.h>

namespace dser
{

    class address_info
    {
        public:
            address_info() = default;
            ~address_info();

            int get_error();

            /// Returns `_ai` if `getaddrinfo` call was succesful
            /// Otherwise returns `nullptr`
            ::addrinfo* get_info();
            void print_info_verbose();
            void print_info_verbose_all();

            ::addrinfo* get_info(const char* node, const char* service);
            const char* get_error_message();

        private:
            ::addrinfo* _ai = nullptr;
            int _error;
    };

}

#endif // __DSER_SOCKET_UTILS_H__

