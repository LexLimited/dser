#include "dser/exception.h"
#include <iostream>

#include <dser/server.h>
#include <dser/http_connection.h>

using namespace dser;

int http::server::serve(std::string_view port, const std::shared_ptr<router> &r)
{
    int fd = this->_socket.open();
    if (fd < 0)
    {
        std::cout << "Server failed to open a socket: " << strerror(errno) << std::endl;
        return fd;
    }

    int err = this->_socket.bind(port.data());
    if (err)
    {
        std::cout << "Server failed to bind a socket: " << strerror(errno) << std::endl;
        return err;
    }

    this->_r = r;

    err = this->_socket.listen();
    if (err) return err;
    while(1)
    {
        fd = this->_socket.accept();
        std::cout << "accepted fd " << fd << std::endl;
        http_connection conn(fd);

        try
        {
            auto http = conn.read_http();
            std::cout << "Read http request from connection" << std::endl;
            std::cout << http.stringify() << std::endl;
        } catch (dser::exception &e)
        {
            std::cout << "Failed to read http request from connection: " << e.what() << std::endl;
        }
    }

    return 0;
}

