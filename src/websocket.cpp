#include "dser/exception.h"
#include "dser/socket.h"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <bitset>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fstream>

#include <dser/assert.h>
#include <dser/websocket.h>
#include <dser/algorithms.h>

#include <iostream>
#include <iterator>
#include <limits>
#include <source_location>
#include <sys/socket.h>

using namespace dser::websocket;

websocket::websocket() {}

websocket::websocket(http::http& hs_request, int fd)
{
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

void print_bits(std::span<std::byte> bytes)
{
    for (const auto byte : bytes)
    {
        std::cout << std::format("{:08b} ", (uint8_t)byte);
    }
    std::cout << std::endl;
}

int websocket::read_frame()
{
    static_assert(sizeof(first_bytes) == 2);

    std::byte buf[12];

    std::cout << "receiving first 2 bytes" << std::endl;
    
    ssize_t read_status = ::recv(this->_fd, buf, 2, MSG_WAITALL);
    if (read_status < 0) throw socket_exception{ errno };
    if (!read_status) return -1;

    std::cout << "First 2 bytes" << std::endl;
    print_bits({ buf, 2 });
    std::cout << "received first 2 bytes" << std::endl;

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

    std::cout << "Reading payload" << std::endl;
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

    int i = 0;
    int err;
    while (should_continue_reading)
    {
        err = this->read_frame();
        if (err)
        {
            std::cout << "Error reading frame" << std::endl;
            this->_message_read_successfully = 0;
            return -1;
        }

        if (this->_frame.final)
        {
            should_continue_reading = 0;
        }

        std::cout << "\n\nRead frame N " << i++ << std::endl;

        std::cout << "Read from websocket" << std::endl;
        std::cout << "Final: " << (unsigned)this->_frame.final << std::endl;
        std::cout << "RSV1: " << (unsigned)this->_frame.rsv1 << std::endl;
        std::cout << "RSV2: " << (unsigned)this->_frame.rsv2 << std::endl;
        std::cout << "RSV3: " << (unsigned)this->_frame.rsv3 << std::endl;
        std::cout << "Mask: " << std::endl;
        print_bits({ (std::byte*)&(this->_frame.mask), 4 });
        std::cout << "Payload length: " << (unsigned)this->_frame.payload_length << std::endl;

        dser::algorithms::apply_xormask(
                &this->_frame.mask, this->_frame.payload.data(),
                sizeof(uint32_t), this->_frame.payload_length);

        std::cout << "Unmasked payload: " << std::endl;
        for (std::byte byte : this->_frame.payload)
        {
            std::cout << (int)byte << std::endl;
        }

        for (std::byte byte : this->_frame.payload)
        {
            std::cout << (char)byte;
        }
        
        std::cout << std::endl;

        /*
        this->_frame.payload.resize(this->_frame.payload.size() + this->_frame.payload.size() % 4);
        unmasked.reserve(this->_frame.payload.size());
  
        const auto transformation = [i = (size_t)0, this](std::byte c) mutable -> std::byte {
            unsigned char byte = (unsigned char)c ^ ((unsigned char*)&_frame.mask)[i % 4];
            return (std::byte)byte;
        };

        uint32_t* payload_ptr = (uint32_t*)this->_frame.payload.data();
        for (size_t i = 0; i < this->_frame.payload.size() / 4; ++i) {
            ((uint32_t*)unmasked.data())[i] = payload_ptr[i] ^ this->_frame.mask;
        }

        this->_frame.payload.resize(this->_frame.payload_length);
        unmasked.resize(this->_frame.payload_length);
        */

    }

    return 0;
}

