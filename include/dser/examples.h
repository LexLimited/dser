#ifndef __DSER_EXAMPLES_H__
#define __DSER_EXAMPLES_H__

#include <cstring>
#include <string>
#include <stdio.h>

#include "assert.h"
#include "file.h"
#include "inet_socket.h"

#if defined(test_google_request)
statistatic_assert(
        0 && "redefinition of dser::test_google_request function - "
        "you might have included examples.h headers multiple times");
#endif

namespace dser::examples
{

    int pdf_request()
    {
        printf("Youtube request test:\n");

        dser::inet_socket sock { AF_INET };
        assert(sock.open() > 0);
        assert_perr(!sock.set_timeout(5000), "Failed to set timeout on socket");

        const char* host = "ocw.nctu.edu.tw";
        const char* port = "80";
        if (sock.connect(host, port))
        {
            printf("Failed to connect to %s:%s\n", host, port);
            printf("Reason: %s\n", strerror(errno));
            return errno;
        } else
        {
            printf("Connected succesfully\n");
        }

        dser::fs::file req_file;
        assert(!req_file.open("../assets/requests/pdf_example.http"));

        ssize_t bytes_sent = ::send(sock.fd(), req_file.data(), req_file.size(), 0);    
        printf("%d bytes sent\n", (int)bytes_sent);

        if (bytes_sent < 0)
        {
            printf("Failed to send: %s\n", strerror(errno));
            return errno;
        }

        if (!bytes_sent)
        {
            printf("Connection was shut down\n");
            return errno;
        }

        const size_t recv_buf_size = 512;
        std::string received_message;
        char *recv_buf = new char[recv_buf_size + 1];
        bzero(recv_buf, recv_buf_size + 1);

        ssize_t recv_status = 0;
        int iterations = 0;
        do {
            recv_status = ::recv(sock.fd(), recv_buf, recv_buf_size, 0);

            if (recv_status == 0)
            {
                printf("\nConnection was shut down\n");
            }
            else if (recv_status < 0)
            {
                printf("\nFailed to read: %s\n", strerror(errno));
            }
            else
            {
                printf("%s", std::string(recv_buf).c_str());         
                received_message += std::string(recv_buf);
            }

        } while (recv_status > 0 && iterations++ < 1000);

        printf("Received status: %d\n", (int)recv_status);
        printf("Received %lu bytes in total\n", received_message.size());

        delete[] recv_buf;
        return 0;
    }

    int google_request()
    {
        printf("Google request test:\n");

        dser::inet_socket sock { AF_INET };
        assert(sock.open() > 0);
        assert_perr(!sock.set_timeout(5000), "Failed to set timeout on socket");

        const char* google_name = "google.com";
        const char* google_port = "80";
        if (sock.connect(google_name, google_port))
        {
            printf("Failed to connect to %s:%s\n", google_name, google_port);
            printf("Reason: %s\n", strerror(errno));
            return errno;
        } else
        {
            printf("Connected succesfully\n");
        }

        dser::fs::file req_file;
        assert(!req_file.open("../assets/requests/google_example.http"));

        ssize_t bytes_sent = ::send(sock.fd(), req_file.data(), req_file.size(), 0);    
        printf("%d bytes sent\n", (int)bytes_sent);

        if (bytes_sent < 0)
        {
            printf("Failed to send: %s\n", strerror(errno));
            return errno;
        }

        if (!bytes_sent)
        {
            printf("Connection was shut down\n");
            return errno;
        }

        const size_t recv_buf_size = 512;
        std::string received_message;
        char *recv_buf = new char[recv_buf_size + 1];
        bzero(recv_buf, recv_buf_size + 1);

        ssize_t recv_status = 0;
        int iterations = 0;
        do
        {
            recv_status = ::recv(sock.fd(), recv_buf, recv_buf_size, 0);

            if (recv_status == 0)
            {
                printf("\nConnection was shut down\n");
            }
            else if (recv_status < 0)
            {
                printf("\nFailed to read: %s\n", strerror(errno));
            }
            else
            {
                printf("%s", std::string(recv_buf).c_str());         
                received_message += std::string(recv_buf);
            }

        } while (recv_status > 0 && iterations++ < 1000);

        printf("Received status: %d\n", (int)recv_status);
        printf("Received %lu bytes in total\n", received_message.size());

        delete[] recv_buf;
        return 0;
    }

}

#endif // __DSER_EXAMPLES_H__

