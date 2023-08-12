#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdio>
#include <dser/socket.h>

#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

void print_sockaddr(sockaddr* sa)
{
    printf("data: %.*s, family: %d\n", sizeof(sa->sa_data), sa->sa_data, sa->sa_family);
}

void print_addrinfo(addrinfo* ai)
{
    printf("------------------------\n");
    printf("address info\n");
    printf("------------------------\n");
    
    printf("socket address:\n");
    print_sockaddr(ai->ai_addr);
    printf("family: %d\n", ai->ai_family);
    printf("flags: %d\n", ai->ai_flags);
    printf("addrlen: %d\n", ai->ai_addrlen);
    printf("protocol: %d\n", ai->ai_protocol);
    printf("socket type: %d\n", ai->ai_socktype);
    printf("canonical name: %s\n", ai->ai_canonname);
    
    printf("------------------------\n");
}

int dser::open_inet6_socket(int family)
{
    int fd = ::socket(family, SOCK_STREAM, 0);
    if (!fd) return 0;

    int option = 1;
    // ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    return fd;
}

int dser::bind_inet_socket(const char* domain, const char* port)
{
    addrinfo *ai;
    addrinfo ai_hints;

    bzero(&ai_hints, sizeof(addrinfo));
    int err = getaddrinfo(domain, port, &ai_hints, &ai);
    if (err) return err;

    // print_addrinfo(ai);
    return 0;
}

int dser::socket::set_timeout(int t)
{
    this->_timeout = t;
    int tm_usecs = 1000 * t;
    struct timeval tv { .tv_sec = tm_usecs / 1'000'000, .tv_usec = tm_usecs % 1'000'000 };

    int err;
    // err = setsockopt(this->_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    // if (err) return errno;
    err = setsockopt(this->_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    return err ? errno : 0;
}

