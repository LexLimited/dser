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
            inet_socket(int family = AF_INET);
            inet_socket(const inet_socket&) = delete;
            inet_socket(inet_socket&& other);

            ~inet_socket();
  
            inline int family() const noexcept { return this->_family; }

            int get_address_info(const char* node, const char* service, ::addrinfo **ai) const;
            int open() override;
            // int close() override;
            int bind(const char* port);
            int listen() override;
            int connect(const char* node, const char* service) override;
            int accept() override;

            inet_socket& operator=(const inet_socket&) = delete;
            inet_socket& operator=(inet_socket&& other);

        private:
            int _family;
    };

}

#endif // __DSER_INET_SOCKET_H__

