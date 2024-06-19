#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <netdb.h>
#include <type_traits>
#include <unistd.h>
#include <sys/socket.h>

#include <dser/socket.h>

namespace dser
{

    void print_sockaddr(sockaddr* sa)
    {
        printf("data: %.*s, family: %d\n", static_cast<int>(sizeof(sa->sa_data)), sa->sa_data, sa->sa_family);
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

    int open_inet6_socket(int family)
    {
        int fd = ::socket(family, SOCK_STREAM, 0);
        if (!fd) return 0;

        int option = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        return fd;
    }

    int bind_inet_socket(const char* domain, const char* port)
    {
        addrinfo *ai;
        addrinfo ai_hints;

        bzero(&ai_hints, sizeof(addrinfo));
        int err = getaddrinfo(domain, port, &ai_hints, &ai);
        if (err) return err;

        // print_addrinfo(ai);
        return 0;
    }

    void swap(socket& s1, socket& s2)
    {
        std::swap(s1._fd, s2._fd);
        std::swap(s1._timeout, s2._timeout);
    }

    socket::socket(dser::socket &&other)
        : _fd(other._fd), _timeout(other._timeout)
    {}

    static int check_fd(const socket& sock)
    {
        if (auto fd = sock.fd() < 0)
        {
            std::cerr << "    Socket's fd is invalid (" << fd << ')' << std::endl;
            return -1;
        }
        return 0;
    }

    static int check_self_info(const socket& sock)
    {
        ::sockaddr_storage sa;
        socklen_t addr_len = sizeof(sa);

        int err = ::getsockname(sock.fd(), (::sockaddr*)&sa, &addr_len);
        if (err)
        {
            std::cerr << "    Failed to get socket name. Reason: " << strerror(errno) << std::endl;
            return err;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        err = ::getnameinfo((::sockaddr*)&sa, addr_len, host, sizeof(host), service, sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV);
        if (err)
        {
            std::cerr << "    Failed to get socket address info. Reason: " << gai_strerror(err) << std::endl;
        }

        std::cout << "    Local socket address: " << host << ':' << service << std::endl;
        return 0;
    }

    static int check_connection(const socket& sock)
    {
        struct sockaddr_storage sa;
        socklen_t addr_len = sizeof(sa);

        int err = ::getsockname(sock.fd(), (::sockaddr*)&sa, &addr_len);
        if (err)
        {
            std::cerr << "    Failed to get socket name. Reason: " << gai_strerror(errno) << std::endl;
            return err;
        }

        return 0;
    }

    static int check_peer_info(const socket& sock)
    {
        ::sockaddr_storage sa;
        socklen_t addr_len = sizeof(sa);

        int err = ::getpeername(sock.fd(), (::sockaddr*)&sa, &addr_len);
        if (err)
        {
            std::cerr << "    Failed to get peer name. Reason: " << strerror(errno) << std::endl;
            return err;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        err = ::getnameinfo((::sockaddr*)&sa, addr_len, host, sizeof(host), service, sizeof(service), NI_NUMERICHOST | NI_NUMERICSERV);
        if (err)
        {
            std::cerr << "    Failed to get peer socket address info. Reason: " << gai_strerror(err) << std::endl;
            return err;
        }

        std::cout << "    Peer socket address: " << host << ':' << service << std::endl;
        return 0;
    }

    int socket::health_check() const
    {
        std::cout << "Socket health check:" << std::endl;
        int err;
        int err_self_info;
        int err_peer_info;

        if ((err = check_fd(*this)) < 0) return err;
        err_self_info = check_self_info(*this);
        if ((err = check_connection(*this))) return err;
        err_peer_info = check_peer_info(*this);

        if (!err && !err_self_info && !err_peer_info)
        {
            std::cout << "    Socket appears to be healthy" << std::endl;
        }
        else
        {
            std::cout << "    Some issues were detected" << std::endl;
        }

        return 0;
    }

    int socket::set_timeout(int t)
    {
        this->_timeout = t;
        int tm_usecs = 1000 * t;
        struct timeval tv { .tv_sec = tm_usecs / 1'000'000, .tv_usec = tm_usecs % 1'000'000 };

        int err;
        err = setsockopt(this->_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        if (err) return errno;
        err = setsockopt(this->_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        return err ? errno : 0;
    }

    socket& socket::operator=(socket &&other)
    {
        swap(*this, other);
        other.terminate();
        return *this;
    }

    void socket::terminate()
    {
        if (this->_fd >= 0)
        {
            ::close(this->_fd);
            this->_fd = -1;
        }
    }

}
