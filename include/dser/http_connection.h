#ifndef __DSER_HTTP_CONNECTION_H__
#define __DSER_HTTP_CONNECTION_H__

#include "connection.h"
#include "http.h"
#include "inet_socket.h"

namespace dser::http
{

    class http_connection : public dser::connection
    {
        public:
            http_connection(int fd)
            {
                this->_sock.set_fd(fd);
            }
            
            ~http_connection() {}

            void read(char* buffer, size_t n) override;
            void write(char* buffer, size_t n) override;
            http read_http();

        private:
            inet_socket _sock;
    };

}

#endif // __DSER_HTTP_CONNECTION_H__

