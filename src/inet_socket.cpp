#include "dser/socket.h"
#include <cstdio>
#include <cstring>
#include <dser/inet_socket.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

using namespace dser;

inet_socket::inet_socket(inet_socket &&other)
    : socket(std::move(other)), _family(other._family)
{}

inet_socket::inet_socket(int family): _family(family) {}

inet_socket::~inet_socket() = default; // { this->close(); }

int inet_socket::open()
{
    return this->_fd = open_inet6_socket(this->_family);
}

/*
int inet_socket::close()
{
    if (this->_fd)
    {
        int err = ::close(this->_fd);
        this->_fd = 0;
        return err;
    }
    return -1;
}
*/

int inet_socket::get_address_info(const char* node, const char* service, ::addrinfo **ai) const
{
    ::addrinfo ai_hints;
    bzero(&ai_hints, sizeof(::addrinfo));
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_family = this->_family;
    ai_hints.ai_flags = AI_PASSIVE;
    return ::getaddrinfo(node, service, &ai_hints, ai);
}

int inet_socket::bind(const char* port)
{
    int err;
    ::addrinfo *ai;
    err = get_address_info("localhost", port, &ai);
    if (err)
    {
        printf("Failed to get address info: %s", gai_strerror(err));
        return err;
    }

    for (::addrinfo* _ai = ai; _ai != nullptr; _ai = _ai->ai_next)
    {
        err = ::bind(this->_fd, _ai->ai_addr, _ai->ai_addrlen);
        if (!err) return 0;
    }

    return err;
}

int inet_socket::listen()
{
    return ::listen(this->_fd, 128);
}

int inet_socket::connect(const char* node, const char* service)
{
    int err;
    ::addrinfo *ai;
    err = get_address_info(node, service, &ai);
    if (err)
    {
        printf("Failed to get address info: %s\n", gai_strerror(err));
        return -1;
    }

    if (!ai)
    {
        printf("ai is null\n");
        return -1;
    }

    for (::addrinfo *_ai = ai; _ai != nullptr; _ai = _ai->ai_next)
    {
        err = ::connect(this->_fd, ai->ai_addr, ai->ai_addrlen);
        if (!err) return 0;
    }

    return err;
}

int inet_socket::accept()
{
    sockaddr conn_sock;
    socklen_t conn_len;
    return ::accept(this->_fd, &conn_sock, &conn_len);
}

inet_socket& inet_socket::operator=(inet_socket &&other)
{
    socket::operator=(std::move(other));
    this->_family = other._family;
    return *this;
}

