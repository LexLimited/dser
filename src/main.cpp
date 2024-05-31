#include <assert.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gnutls/gnutls.h>
#include <iostream>
#include <libpq-fe.h>
#include <limits>
#include <memory>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <sys/poll.h>
#include <thread>
#include <unistd.h>
#include <coroutine>
#include <strings.h>
#include <format>

#include <net/ethernet.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <poll.h>

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <gnutls/crypto.h>

#include <ranges>
#include "omp.h"

#include <dser/exception.h>
#include <dser/http_connection.h>
#include <dser/postgres.h>
#include <dser/postgres_app.h>
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
// #include <dser/postgres.h>
#include <dser/postgres_command.h>
#include <dser/server.h>
#include <dser/router.h>
#include <dser/pipe.h>
#include <dser/log.h>
#include <dser/path.h>

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
        std::cout << "read message of size " << ws.message().size() << ":" << std::endl;
        for (std::byte byte : ws.message()) {
            std::cout << (char)byte;
        }
        std::cout << std::endl;
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

auto postgres_create_tables(dser::postgres::app &app)
{
    std::string query = dser::postgres::read_query_from_file("create_tables.sql");
    return dser::postgres::exec_cmd(app, query.c_str());
}

int test_postgres()
{
    dser::pipe p(dser::pipe::pipe_type::READ);
    dser::pipe::result pipe_result;
    pipe_result = p.exec_complete("pg_ctl -D db/data -l log.log");
    if (pipe_result.error)
    {
        dser::log::debug("Failed to start pg server:", pipe_result.output);
        return pipe_result.error;
    }

    dser::postgres::app app;
    if (!app.connection_ok()) {
        std::cout << "Connection error" << std::endl;
        std::cout << PQerrorMessage(app.conn()) << std::endl;
        return -1;
    }

    auto res = postgres_create_tables(app);
    auto status = PQresultStatus(res.result());
    if (status != PGRES_COMMAND_OK)
    {
        std::cout << "status: " << status << std::endl;
        std::cout << "Error creating tables:" << std::endl;
        std::cout << PQresultErrorMessage(res.result()) << std::endl;
    }

    res = dser::postgres::exec_cmd(app, "SELECT * FROM users");
    status = PQresultStatus(res.result());
    if (status != PGRES_TUPLES_OK)
    {
        std::cout << "status: " << status << std::endl;
        std::cout << "Error selecting from users:" << std::endl;
        std::cout << PQresultErrorMessage(res.result()) << std::endl;
    }  else
    {
        std::cout << "Result" << std::endl;
        std::cout << "Number of tuples: " << PQntuples(res.result()) << std::endl;
        // PQprint(stdout, res.result(), &opts);
    }

    pipe_result = p.exec_complete("pg_ctl stop -D db/data");
    if (pipe_result.error)
    {
        throw dser::exception("Failed to stop pg server");
    }

    return 0;
}

void pipe_test()
{
    dser::pipe read_pipe(dser::pipe::pipe_type::READ);
    dser::pipe write_pipe(dser::pipe::pipe_type::WRITE);

    read_pipe.exec("echo \"c\na\nb\"");
    write_pipe.exec("sort");

    int fds[] = { read_pipe.fd(), write_pipe.fd() };
    int err = ::pipe(fds);
    if (err)
    {
        dser::log::debug("Failed to pipe:", strerror(errno));
    }

    dser::log::debug("Succeeded with piping:",
            read_pipe.read_to_string(),
            write_pipe.read_to_string());
}

int main()
{
    dser::path p("/a/b/*");

    std::cout << "Path segments:" << std::endl;
    for (const auto& c : p.get_segments())
    {
        std::cout << c << std::endl;
    }

    return 0;

    dser::postgres::app app;
    if (!app.connection_ok()) {
        std::cout << "Connection error" << std::endl;
        std::cout << PQerrorMessage(app.conn()) << std::endl;
        return -1;
    }

    auto res = postgres_create_tables(app);
    auto status = PQresultStatus(res.result());
    if (status != PGRES_COMMAND_OK)
    {
        std::cout << "status: " << status << std::endl;
        std::cout << "Error creating tables:" << std::endl;
        std::cout << PQresultErrorMessage(res.result()) << std::endl;
    }

    res = dser::postgres::exec_cmd(app, "SELECT * FROM users");
    status = PQresultStatus(res.result());
    if (status != PGRES_TUPLES_OK)
    {
        std::cout << "status: " << status << std::endl;
        std::cout << "Error selecting from users:" << std::endl;
        std::cout << PQresultErrorMessage(res.result()) << std::endl;
    }  else
    {
        std::cout << "Result" << std::endl;
        std::cout << "Number of tuples: " << PQntuples(res.result()) << std::endl;
        // PQprint(stdout, res.result(), &opts);
    }

    return 0;

    dser::signals::handle_signals();
    hosting_test();
}

