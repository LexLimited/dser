#ifndef __DSER_INET_SOCKET_H__
#define __DSER_INET_SOCKET_H__

#include "socket.h"
#include <sys/socket.h>

namespace dser {

    class inet_socket : ::dser::socket {
        public:
            inet_socket(int family = AF_INET);
            ~inet_socket();
  
            inline int family() const noexcept { return this->_family; }
            int fd() const noexcept;

            int open() override;
            int close() override;
            int bind(const char* port);
            int listen() override;
            int connect(const char* node, const char* service) override;
            int accept() override;

        private:
            int _family;
            int _fd;
    };

}

#endif // __DSER_INET_SOCKET_H__

