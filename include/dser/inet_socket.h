#ifndef __DSER_INET_SOCKET_H__
#define __DSER_INET_SOCKET_H__

#include "socket.h"

#include <netdb.h>
#include <sys/socket.h>

namespace dser
{

    class inet_socket : public ::dser::socket
    {
        public:
            static inet_socket new_client(const char* port);
            static const int DEFAULT_FAMILY = AF_INET6; /// defaults to IPv4 protocol

        public:
            inet_socket(int family = DEFAULT_FAMILY);
            inet_socket(const inet_socket&) = delete;
            inet_socket(inet_socket&& other);

            ~inet_socket();
  
            int family() const noexcept;
            void set_family(int family);
            void set_allow_change_family(bool value) noexcept;

            int get_address_info(const char* node, const char* service, ::addrinfo **ai);
            
            int open() override;
            int bind(const char* port);
            int listen() override;
            int connect(const char* node, const char* service) override;
            int accept() override;

            inet_socket& operator=(const inet_socket&) = delete;
            inet_socket& operator=(inet_socket&& other);

        private:
            int _family;
            bool _allow_change_family = false;
    };

}

#endif // __DSER_INET_SOCKET_H__

