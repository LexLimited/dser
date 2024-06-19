#include "dser/exception.h"
#include <asm-generic/errno.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <utility>
#include <sys/socket.h>

#include <dser/inet_socket.h>

using namespace dser;

inet_socket inet_socket::new_client(const char* port)
{
    inet_socket client;
    int status;

    if ((status = client.open()) < 0)
        throw dser::exception("Failed to open socket for client");

    if ((status = client.bind(port)))
        throw dser::exception("Failed to bind client socket");

    return client;
}

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

int inet_socket::family() const noexcept
{
    return this->_family;
}

void inet_socket::set_family(int family)
{
    if (!this->_allow_change_family)
        throw dser::exception("[set_family]: Inet socket is not allowed to change family");
    this->_family = family;
}

void inet_socket::set_allow_change_family(bool value) noexcept
{
    this->_allow_change_family = value;
}

static int get_address_info(
        const char* node,
        const char* service,
        ::addrinfo** ai,
        int ipv)
{
    if (ipv != AF_INET && ipv != AF_INET6)
    {
        std::cerr << "Invalid protocol supplied to `get_address_info`: " << ipv << std::endl;
    }
    
    static const auto afamily_string = [](int family) {
        switch (family)
        {
            case AF_INET: return "IPv4";
            case AF_INET6: return "IPv6";
            default: return "INVALID";
        }
    };

    ::addrinfo ai_hints;
    bzero(&ai_hints, sizeof(ai_hints));
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_family = ipv;
    ai_hints.ai_flags = AI_PASSIVE;

    int status = ::getaddrinfo(node, service, &ai_hints, ai);
    if (status)
    {
        std::cerr << "Failed to get address info for family "
            << afamily_string(ipv)
            << "."
            << std::endl;

        return status;
    }

    return status;
}

int inet_socket::get_address_info(const char* node, const char* service, ::addrinfo **ai)
{
    static const auto afamily_string = [](int family) {
        switch (family)
        {
            case AF_INET: return "IPv4";
            case AF_INET6: return "IPv6";
            default: return "INVALID";
        }
    };

    ::addrinfo ai_hints;
    bzero(&ai_hints, sizeof(ai_hints));
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_family = this->_allow_change_family ? AF_UNSPEC : this->_family;
    ai_hints.ai_flags = AI_PASSIVE;

    int status = ::getaddrinfo(node, service, &ai_hints, ai);
    if (status)
    {
        std::cerr << "Failed to get address info for family "
                  << afamily_string(this->_family)
                  << "."
                  << std::endl;
        
        if (!this->_allow_change_family)
        {
            std::cout << "Socket is not allowed to change the version of IP protocol."
                         " To enable it, call `set_allow_change_family(true)`."
                      << std::endl;
        }
        else
        {
            this->_family = 
        }

        return status;
    }

    if ((*ai)->ai_family != this->_family)
    {
        std::cout << "Address info for requested family ("
                  << afamily_string(this->_family)
                  << ") was not found, but was found for "
                  << afamily_string((*ai)->ai_family)
                  << ". Socket family changed."
                  << std::endl;
        this->_family = (*ai)->ai_family;
    }

    return 0;
}

int inet_socket::bind(const char* port)
{
    int err;
    ::addrinfo *ai;
    err = get_address_info("localhost", port, &ai);
    if (err)
    {
        printf("[bind]: Failed to get address info: %s", gai_strerror(err));
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

const char* connect_errno_to_string(int e)
{
#if defined(CASE_EXACT)
    static_assert(0, "Attempted CASE_EXACT macro redefinition")
#endif
#define CASE_EXACT(const_name) case const_name: return #const_name
    switch (e)
    {
        CASE_EXACT(EACCES);
        CASE_EXACT(EPERM);
        CASE_EXACT(EADDRINUSE);
        CASE_EXACT(EADDRNOTAVAIL);
        CASE_EXACT(EAFNOSUPPORT);
        CASE_EXACT(EAGAIN);
        CASE_EXACT(EALREADY);
        CASE_EXACT(EBADF);
        CASE_EXACT(ECONNREFUSED);
        CASE_EXACT(EFAULT);
        CASE_EXACT(EINPROGRESS);
        CASE_EXACT(EINTR);
        CASE_EXACT(EISCONN);
        CASE_EXACT(ENETUNREACH);
        CASE_EXACT(ENOTSOCK);
        CASE_EXACT(EPROTOTYPE);
        CASE_EXACT(ETIMEDOUT);
        CASE_EXACT(EINVAL);
        default: return "UNKNOWN";
    }
#undef CASE_EXACT
}

int inet_socket::connect(const char* node, const char* service)
{
    if (this->_fd < 0)
    {
        std::cerr << "[connect]: inet_socket connection error: called with an invalid file descriptor ("
                  << this->_fd
                  << ")";
        return -1;
    }

    int err;
    ::addrinfo *ai;

    err = get_address_info(node, service, &ai);
    if (err || !ai)
    {
        return -1;
    }

    for (::addrinfo *_ai = ai; _ai != nullptr; _ai = _ai->ai_next)
    {
        if (_ai->ai_addrlen != sizeof(::addrinfo))
            continue;

        err = ::connect(this->_fd, _ai->ai_addr, sizeof(::addrinfo) /* _ai->ai_addrlen */);
        if (!err)
        {
            ::freeaddrinfo(ai);
            return 0;
        }
    }

    return err;
}

int inet_socket::accept()
{
    sockaddr conn_sock;
    socklen_t conn_len;
    return ::accept(this->_fd, &conn_sock, &conn_len);
}

ssize_t inet_socket::send(const char* data, size_t data_len)
{
    return ::send(this->_fd, data, data_len, MSG_NOSIGNAL);
}

ssize_t inet_socket::recv(void* buf, size_t buf_len)
{
    return ::recv(this->_fd, buf, buf_len, MSG_ERRQUEUE);
}

inet_socket& inet_socket::operator=(inet_socket &&other)
{
    socket::operator=(std::move(other));
    this->_family = other._family;
    return *this;
}

