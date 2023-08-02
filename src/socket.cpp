#include <cstdio>
#include <dser/socket.h>

#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

void print_sockaddr(sockaddr* sa) {
    printf("data: %.*s, family: %d\n", sizeof(sa->sa_data), sa->sa_data, sa->sa_family);
}

void print_addrinfo(addrinfo* ai) {
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

int dser::open_inet6_socket(int family) {
    return ::socket(family, SOCK_STREAM, 0);
}

int dser::bind_inet_socket(const char* domain, const char* port) {
    addrinfo *ai;
    addrinfo ai_hints;

    bzero(&ai_hints, sizeof(addrinfo));
    int err = getaddrinfo(domain, port, &ai_hints, &ai);
    if (err) return err;

    // print_addrinfo(ai);
    return 0;
}

