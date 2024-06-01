#include <cstring>
#include <dser/http_connection.h>
#include <iostream>
#include <sys/poll.h>
#include <sys/socket.h>

#include <dser/byte_buffer.h>
#include <dser/exception.h>
#include <dser/http_parser.h>

namespace dser::http
{

    void http_connection::read(char* buffer, size_t n)
    {
        ::recv(this->_sock.fd(), buffer, n, 0);
    }
    
    void http_connection::write(char* buffer, size_t n)
    {
        ::send(this->_sock.fd(), buffer, n, 0);
    }
    
    http http_connection::read_http()
    {
        /*
        pollfd pfd;
        pfd.events = POLLIN;
        */

        int read_timeout = 3000;
        // int poll_timeout = 3000;

        dser::byte_buffer buffer(1024);

        int poll_status = 0;
        ssize_t read_status;
        
        int to_errno = this->_sock.set_timeout(read_timeout);
        if (to_errno)
        {
            std::cout << "Failed to set timeout: " << strerror(to_errno) << std::endl;
        }
            
        do {
            read_status = ::recv(this->_sock.fd(), buffer.head(), buffer.cap_remaining(), 0);
            if (read_status < 0)
            {
                std::cout << "Failed to read from connection: " << strerror(errno) << std::endl;
                break;
            } else if (!read_status)
            {
                std::cout << "Read timeout" << std::endl;
                break;
            }
        } while (poll_status > 0);

        /*
        do {
            poll_status = ::poll(&pfd, 1, poll_timeout);
            if (poll_status < 0)
            {
                std::cout << "poll error: " << strerror(errno) << std::endl;
            }
        } while (poll_status > 0);
        */

        http_parser parser;
        http http;

        std::cout << "Buffer:" << std::endl;
        std::cout << buffer.string() << std::endl;

        std::string buf_str = buffer.string();
        auto err = parser.parse_http_string(buf_str, http);
        
        if (err != http_parser::http_parser_error::NO_ERROR)
        {
            throw dser::exception("falied to parse http");
        }

        return http;
    }

}

