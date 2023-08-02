#include <assert.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <stdio.h>
#include <string>

#include <net/ethernet.h>
#include <strings.h>
#include <sys/utsname.h>
#include <poll.h>

#include "omp.h"

#include <dser/json_parser.h>
#include <dser/file.h>
#include <dser/socket.h>
#include <dser/inet_socket.h>
#include <sys/socket.h>
#include <unistd.h>
#include <coroutine>

#define LOG_ERRNO(errno) printf("%s\n", strerror(errno))

void assert_perr(int success, const char* msg) {
    if (!success) {
        va_list list;
        perror(msg);
        assert(success);
    }
}

int hosting_test() {
    dser::inet_socket server;
    assert_perr(server.open() > 0, "Failed to open");
    assert_perr(!server.bind("3000"), "Failed to bind");
    assert_perr(!server.listen(), "Failed to listen");
}

int google_test() {
    int err;

    dser::inet_socket sock { AF_INET };
    assert(sock.open() > 0);
 
    const char* google_name = "google.com";
    const char* google_port = "80";
    
    if (sock.connect(google_name, google_port)) {
        printf("Failed to connect to %s:%s\n", google_name, google_port);
        printf("Reason: %s\n", strerror(errno));
        return -1;
    }
  
    dser::fs::file req_file;
    req_file.open("../assets/requests/google_example.http");

    ssize_t bytes_sent = ::send(sock.fd(), req_file.data(), req_file.size(), 0);    
    printf("%d bytes sent\n", (int)bytes_sent);
    if (bytes_sent < 0) {
        printf("Failed to send: %s\n", strerror(errno));
    }

    const size_t recv_buf_size = 512;
    std::string received_message;
    char *recv_buf = new char[recv_buf_size + 1];
    bzero(recv_buf, recv_buf_size + 1);

    ssize_t recv_status = 0;
    int iterations = 0;
    do {
        recv_status = ::recv(sock.fd(), recv_buf, recv_buf_size, 0);
        
        if (recv_status == 0) { printf("Connection was shut down\n"); }
        else if (recv_status < 0) { printf("Failed to read: %s\n", strerror(errno)); }
        else {
            printf("%s", std::string(recv_buf).c_str());         
            received_message += std::string(recv_buf);
        }
    } while (recv_status > 0 && iterations++ < 1000);

    printf("Received status: %d\n", (int)recv_status);

    printf("Received %lu bytes in total\n", received_message.size());
    // printf("Message: %s\n", received_message.c_str());
    
    delete[] recv_buf;
    return 0;
}

int main() {
    // google_test();
    hosting_test();
}

/*
int main(char**, char[3] != char*, char[] == char*?) {
    int err;

    dser::inet_socket server;
    dser::inet_socket client;

    err = server.open();
    assert(err != 0 && "Failed to open server");
    err = server.bind("8001");
    if (err) {
        printf("Failed to bind: %s\n", strerror(errno));
        return -1;
    }

    err = client.open();
    assert(err != 0 && "Failed to open client");
    int conn = server.accept();
    if (conn == 0) {
        printf("Failed to accept connection\n");
        return -1;
    }

    char write_buf[512] = {0};
    char read_buf[512] = {0};
    strcpy(write_buf, "Hello, world");
    ssize_t msg_len;

    err = server.listen();
    if (err) {
        printf("Failed to listen: %s\n", strerror(errno));
        return -1;
    }

    err = client.connect("localhost", "8001");
    if (err) {
        printf("Failed to connect to server: %s\n", strerror(errno));
        return -1;
    }

    printf("Writing to %d\n", server.fd());
    msg_len = ::send(server.fd(), write_buf, sizeof(write_buf), 0);
    if (msg_len < 0) {
        printf("Failed to send: %s\n", strerror(errno));
    } else printf("Sent %ld bytes\n", msg_len);

    printf("Reading from %d\n", server.fd());
    msg_len = ::recv(server.fd(), read_buf, sizeof(read_buf), 0);
    if (msg_len < 0) {
        printf("Failed to send: %s\n", strerror(errno));
    } else printf("Received %ld bytes\n", msg_len);

    printf("message sent: %s\n", write_buf);
    printf("message recv: %s\n", read_buf);

    return 0;
}
*/

