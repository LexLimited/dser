#include <assert.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gnutls/gnutls.h>
#include <iostream>
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

#include "dser/http_parser.h"
#include "dser/utils.h"
#include "omp.h"

#include <dser/assert.h>
#include <dser/json_parser.h>
#include <dser/file.h>
#include <dser/socket.h>
#include <dser/inet_socket.h>
#include <dser/http.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <gnutls/crypto.h>

#include <dser/examples.h>
#include <dser/examples.h>

#include <ranges>

dser::http::http create_websocket_handshake_response(dser::http::http& req) {
    dser::http::http h;

    h.set_status_code(101);
    h.set_status("Switching Protocols");

    h.set_header("Upgrade", "websocket");
    h.set_header("Connection", "Upgrade");
    
    std::cout << "SEC-WEBSOCKET-Key" << std::endl;
    std::cout << req.header("Sec-WebSocket-Key") << std::endl;

    h.set_header("Sec-WebSocket-Key", req.header("Sec-WebSocket-Key"));

    std::cout << "REQUEST HEADERS" << std::endl;

    for (const auto& [key, value] : h.headers()) {
        std::cout << key << ", " << value << std::endl;
    }

    std::cout << "=================================" << std::endl;

    gnutls_hash_hd_t handle = nullptr;
    gnutls_digest_algorithm_t sha1 = gnutls_digest_algorithm_t::GNUTLS_DIG_SHA1;
    
    assert(!gnutls_hash_init(&handle, sha1));
    std::string sec = req.header("Sec-WebSocket-Key") + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    assert(!gnutls_hash(handle, sec.data(), sec.size()));

    std::string digest;
    digest.resize(20);
    gnutls_hash_output(handle, digest.data());

    const unsigned char *base64_input = (const unsigned char*)digest.data();
    std::string base64_output;
    base64_output.resize(4 * (digest.size() / 3 + bool(digest.size() % 3)));
    EVP_EncodeBlock((unsigned char*)base64_output.data(), base64_input, digest.size());    
    h.set_header("Sec-WebSocket-Accept", base64_output);
   
    std::cout << "Response http:" << std::endl;
    std::cout << h.stringify() << std::endl;

    return h;
}

int handle_connection(int fd) {
    std::string buf;
    buf.resize(1024);
    ssize_t read_status = ::recv(fd, buf.data(), buf.size(), 0);

    if (read_status < 0) {
        perror("Failed to read");
        return errno;
    }

    if (!read_status) {
        printf("Connection closed\n");
        return 0;
    }

    dser::http::http_parser parser;
    dser::http::http req;
    auto err = parser.parse_http_string(buf, req);
    if (err != dser::http::http_parser::http_parser_error::NO_ERROR) {
        std::cout << "error parsing http: " << dser::http::http_parser::strerror(err) << std::endl;
        return -1;
    }

    printf("\n-----------------------------------\nReceived data:\n%.*s\n------------------------------------\n",
            (int)buf.size(), buf.c_str());
    printf("\n-----------------------------------\nParsed request:\n%s\n--------------------------------------------\n",
            req.stringify().c_str());
    printf("\n-----------------------------------\nRequest headers:\n");
    
    for (const auto& [key, value] : req.headers()) {
        printf("%s:%s\n", key.c_str(), value.c_str());
    }

    dser::http::http res = create_websocket_handshake_response(req);
    std::string res_str = res.stringify();
    ssize_t send_status = ::send(fd, res_str.data(), res_str.size(), 0);
    dser::assert_perr(send_status > 0 && "Failed to send a response");

    while (1) {
        read_status = ::recv(fd, buf.data(), buf.size(), 0);
        if (read_status < 0) {
            printf("Error reading from connection\n");
            return read_status;
        } else if (!read_status) {
            printf("Connection was shut closed\n");
            return 0;
        } else {
            std::cout << "Received data from connectio" << std::endl;
            std::cout << std::string_view( buf.data(), read_status ) << std::endl;
        }

        dser::sleep_ms(16);
    }

    return read_status;
}

int hosting_test() {
    /*
    const unsigned char *base64_data = (const unsigned char*)"aaa";
    unsigned char base64_test[4];

    EVP_EncodeBlock(base64_test, base64_data, 3);    

    gnutls_hash_hd_t handle = nullptr;
    gnutls_digest_algorithm_t sha1 = gnutls_digest_algorithm_t::GNUTLS_DIG_SHA1;
    assert(!gnutls_hash_init(&handle, sha1));
        
    const char* text = "hello gnutls";
    assert(!gnutls_hash(handle, text, std::strlen(text)));

    std::string digest {20};
    gnutls_hash_output(handle, digest.data());

    gnutls_datum_t encode_input { (unsigned char*)digest.data(), 20 };
    gnutls_datum_t encode_output;

    assert(gnutls_hex_encode2(&encode_input, &encode_output) == GNUTLS_E_SUCCESS);

    printf("digest: %.*s\n", encode_output.size, encode_output.data);
    putchar('\n');

    return 0;
    */

    dser::inet_socket server;
    dser::assert_perr(server.open() > 0, "Failed to open");
    dser::assert_perr(!server.set_timeout(1000), "Failed to set timeout");
    dser::assert_perr(!server.bind("3000"), "Failed to bind");
    dser::assert_perr(!server.listen(), "Failed to listen");

    while(1) {
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

void display_startup_message() {
    printf("Application started\n");
}

int main() {
    /*

    EXAMPLE FROM SPECS:

    std::string key = "dGhlIHNhbXBsZSBub25jZQ==";
    std::string guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
   
    -> s3pPLMBiTxaQ9kYGzzhZRbK+xOo=

    */

    // display_startup_message();

    // dser::examples::google_request();
    // dser::examples::pdf_request();
    hosting_test();
}

