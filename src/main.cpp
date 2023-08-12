#include <assert.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gnutls/gnutls.h>
#include <iostream>
#include <limits>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <sys/poll.h>
#include <unistd.h>
#include <coroutine>
#include <strings.h>
#include <format>

#include <net/ethernet.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <poll.h>

#include "omp.h"

#include <dser/http_parser.h>
#include <dser/utils.h>
#include <dser/assert.h>
#include <dser/json_parser.h>
#include <dser/file.h>
#include <dser/socket.h>
#include <dser/inet_socket.h>
#include <dser/http.h>
#include <dser/examples.h>
#include <dser/examples.h>
#include <dser/websocket.h>
#include <dser/signals.h>
#include <dser/crypto.h>

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <gnutls/crypto.h>

#include <ranges>

dser::http::http create_websocket_handshake_response(dser::http::http& req)
{
    dser::http::http h;

    h.set_status_code(101);
    h.set_status("Switching Protocols");

    h.set_header("Upgrade", "websocket");
    h.set_header("Connection", "Upgrade");
    h.set_header("Sec-WebSocket-Key", req.header("Sec-WebSocket-Key"));
    
    std::string sec = req.header("Sec-WebSocket-Key") + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string sha = dser::crypto::sha1(sec);
    std::string b64 = dser::crypto::base64_encode(sha);

    h.set_header("Sec-WebSocket-Accept", b64);

    return h;
}

int handle_connection(int fd)
{
    std::string buf;
    buf.resize(1024);
    ssize_t read_status = ::recv(fd, buf.data(), buf.size(), 0);

    if (read_status < 0)
    {
        perror("Failed to read");
        return errno;
    }

    if (!read_status)
    {
        printf("Connection closed\n");
        return 0;
    }

    dser::http::http_parser parser;
    dser::http::http req;
    auto err = parser.parse_http_string(buf, req);
    if (err != dser::http::http_parser::http_parser_error::NO_ERROR)
    {
        std::cout << "error parsing http: " << dser::http::http_parser::strerror(err) << std::endl;
        return -1;
    }

    std::cout << "\n\n" << "REQUEST:" << req.stringify() << "\n\n" << std::endl;

    dser::http::http res = create_websocket_handshake_response(req);
    std::string res_str = res.stringify();
    ssize_t send_status = ::send(fd, res_str.data(), res_str.size(), 0);
    dser::assert_perr(send_status > 0 && "Failed to send a response");

    dser::websocket::websocket ws { req, fd };

    try
    {
        ws.read();
    } catch(const dser::socket_exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return read_status;
}

int hosting_test()
{
    dser::inet_socket server;
    dser::assert_perr(server.open() > 0, "Failed to open");
    dser::assert_perr(!server.set_timeout(1000), "Failed to set timeout");
    dser::assert_perr(!server.bind("3000"), "Failed to bind");
    dser::assert_perr(!server.listen(), "Failed to listen");

    while(1)
    {
        int fd = server.accept();
        
        if (fd <= 0) {
            printf("Failed to accept: %s\n", strerror(errno));
        } else {
            printf("Connection accepted: %d\n", fd);
            handle_connection(fd);
        }

        dser::sleep_ms(10);
    }

    return 0;
}

void display_startup_message()
{
    printf("Application started\n");
}

#include <libpq-fe.h>

PGconn* postgres_connect() {
    PGconn* conn = PQconnectdb("postgres://lex@localhost:5432/lex");
    if (!conn)return nullptr;

    ConnStatusType status = PQstatus(conn);
    if (status != CONNECTION_OK)
    {
        std::cout << "Connection error: " << PQerrorMessage(conn) << std::endl;
        return nullptr;
    }
    return conn;
}

int main()
{
    PGconn* conn = postgres_connect();
    if (!conn) return -1;
    std::cout << "Connection OK" << std::endl;

    return 0;

    dser::signals::handle_signals();
    hosting_test();
}

