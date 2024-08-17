#include <cstdio>
#include <cstring>
#include <errno.h>

#include <dser/assert.h>
#include <dser/websocket.h>
#include <dser/algorithms.h>
#include <dser/exception.h>
#include <dser/socket.h>

#include <iostream>
#include <sys/socket.h>

using namespace dser::websocket;

websocket::websocket() {}

websocket::websocket(http::http& hs_request, int fd)
{
    (void)hs_request;
    this->_fd = fd;
}

struct first_bytes {
    uint8_t opcode  : 4;
    uint8_t rsv3    : 1;
    uint8_t rsv2    : 1;
    uint8_t rsv1    : 1;
    uint8_t fin     : 1;
    uint8_t len     : 7;
    uint8_t mask    : 1;
};

int websocket::read_frame()
{
    static_assert(sizeof(first_bytes) == 2);

    std::byte buf[12];

    ssize_t read_status = ::recv(this->_fd, buf, 2, MSG_WAITALL);
    if (read_status < 0) throw socket_exception{ errno };
    if (!read_status) return -1;

    first_bytes const &first_bytes = reinterpret_cast<struct first_bytes const &>(buf);
    this->_frame.final = first_bytes.fin;
    this->_frame.rsv1 = first_bytes.rsv1;
    this->_frame.rsv2 = first_bytes.rsv2;
    this->_frame.rsv3 = first_bytes.rsv3;
    this->_frame.masked = first_bytes.mask;
    uint8_t payload_length = first_bytes.len;
    uint8_t n_bytes_extended = 0;
    uint8_t n_bytes_extended_len = 0;
    
    n_bytes_extended += this->_frame.masked ? 4 : 0;
    if (payload_length == 126)
    {
        n_bytes_extended_len = 2;
    } else if (payload_length > 126)
    {
        n_bytes_extended_len = 4;
    }
    n_bytes_extended += n_bytes_extended_len;

    if (n_bytes_extended)
    {
        read_status = ::recv(this->_fd, buf, n_bytes_extended, MSG_WAITALL);
        if (read_status < 0) throw socket_exception{ errno };
        if (!read_status) return -1;
    }
    
    if (payload_length < 126)
    {
        this->_frame.payload_length = payload_length;
    } else if (payload_length == 126)
    {
        this->_frame.payload_length = ntohs(reinterpret_cast<uint16_t*>(buf)[0]);
    } else if (payload_length > 126)
    {
        this->_frame.payload_length = be64toh(reinterpret_cast<uint64_t*>(buf)[0]);
    }

    this->_frame.mask = reinterpret_cast<uint32_t*>(buf + n_bytes_extended_len)[0];
    this->_frame.payload.resize(this->_frame.payload_length);

    read_status = ::recv(
            this->_fd,
            this->_frame.payload.data(),
            this->_frame.payload_length,
            MSG_WAITALL);
    if (read_status < 0)
    {
        throw socket_exception{ errno };
    }

    return 0;
}

int websocket::read() {
    int should_continue_reading = 1;
    int err;

    this->_message.clear();
    while (should_continue_reading)
    {
        err = this->read_frame();
        if (err)
        {
            std::cout << "Error reading frame" << std::endl;
            this->_message_read_successfully = 0;
            return -1;
        }
        
        dser::algorithms::apply_xormask(
                &this->_frame.mask,
                this->_frame.payload.data(),
                sizeof(uint32_t),
                this->_frame.payload_length);
        
        this->_message.insert(
                this->_message.end(),
                this->_frame.payload.begin(),
                this->_frame.payload.end());
        
        if (this->_frame.final)
        {
            should_continue_reading = 0;
        }
    }

    return 0;
}

