#ifndef __DSER_SOCKET_H__
#define __DSER_SOCKET_H__

#include <cstring>
#include <source_location>
#include <string>
#include "exception.h"

namespace dser
{

    class socket_exception : public ::dser::exception
    {
        public:
            socket_exception(int error_code, std::source_location loca = std::source_location::current()): exception(std::strerror(error_code), loca) {}
    };

    int open_inet6_socket(int family);
    int bind_inet_socket(const char* domain, const char* port);

    class socket
    {
        public:
            socket() = default;
            socket(const socket&) = delete;
            socket(socket &&other);

            virtual ~socket() { this->terminate(); };

            virtual int open() = 0;
            // virtual int close() = 0;
            virtual int listen() = 0;
            virtual int connect(const char* node, const char* servicef) = 0;
            virtual int accept() = 0;
            int health_check() const;

            inline int fd() const noexcept { return this->_fd; }
            
            inline void set_fd(int fd) noexcept
            {
                // this->close();
                this->terminate();
                this->_fd = fd;
            }

            // set timeout in milliseocnds
            int set_timeout(int t);

            socket& operator=(const socket&) = delete;
            socket& operator=(socket &&other);

        protected:
            int _fd = -1;
            int _timeout = 0; // snd / rcv timeout in milliseocnds
    
            friend void swap(socket&, socket&);

        private:
            void terminate();
    };

    void swap(socket&, socket&);

}

#endif // __DSER_SOCKET_H__

