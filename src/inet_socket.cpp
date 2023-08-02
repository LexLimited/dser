#include "dser/socket.h"
#include <cstdio>
#include <cstring>
#include <dser/inet_socket.h>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace dser;

inet_socket::inet_socket(int family): _family(family), _fd(0) {}

inet_socket::~inet_socket() { this->close(); }

int inet_socket::fd() const noexcept { return this->_fd; }

int inet_socket::open() {
    return this->_fd = open_inet6_socket(this->_family);
}

int inet_socket::close() {
    if (this->_fd) {
        int err = ::close(this->_fd);
        this->_fd = 0;
        return err;
    }
    return -1;
}

static int get_address_info(const char* node, const char* service, ::addrinfo **ai) {
    ::addrinfo ai_hints;
    bzero(&ai_hints, sizeof(::addrinfo));
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_family = AF_INET;
    ai_hints.ai_flags = AI_PASSIVE;
    // ai_hints->
    // // google.com, "443", ... -> 0 | error
    return ::getaddrinfo(node, service, &ai_hints, ai);
}

int inet_socket::bind(const char* port) {
#if 0
    int err;
    ::addrinfo *ai;
    ::addrinfo ai_hints;
    bzero(&ai_hints, sizeof(::addrinfo));
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_flags = AI_PASSIVE;
#endif

    int err;
    ::addrinfo *ai;
    err = get_address_info("localhost", port, &ai);
    if (err) {
        printf("Failed to get address info: %s", gai_strerror(err));
        return err;
    }

    for (::addrinfo* _ai = ai; _ai != nullptr; _ai = _ai->ai_next) {
        printf("Bind: fd: %d, addr: %p, addrlen: %d\n", this->_fd, (void*)_ai->ai_addr, _ai->ai_addrlen);
        err = ::bind(this->_fd, _ai->ai_addr, _ai->ai_addrlen);
        if (!err) return 0;
    }

    return err;
}

int inet_socket::listen() {
    return ::listen(this->_fd, 128);
}

int inet_socket::connect(const char* node, const char* service) {
    int err;
    ::addrinfo *ai;
    err = get_address_info(node, service, &ai);
    if (err) {
        printf("Failed to get address info: %s\n", gai_strerror(err));
        return -1;
    }

    if (!ai) {
        printf("ai is null\n");
        return -1;
    }

    for (::addrinfo *_ai = ai; _ai != nullptr; _ai = _ai->ai_next) {
        err = ::connect(this->_fd, ai->ai_addr, ai->ai_addrlen);
        if (!err) return 0;
    }

    return err;
}

int inet_socket::accept() {
    sockaddr conn_sock;
    socklen_t conn_len;
    return ::accept(this->_fd, &conn_sock, &conn_len);
}
