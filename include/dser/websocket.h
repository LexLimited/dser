#ifndef __DSER_WEBSOCKET_H__
#define __DSER_WEBSOCKET_H__

#include <cstdint>
#include <vector>

#include "inet_socket.h"
#include "http.h"

namespace dser::websocket
{

    struct ws_frame
    {
        uint64_t payload_length;
        uint_fast8_t masked;
        uint32_t mask;
        uint_fast8_t final;
        uint_fast8_t rsv1;
        uint_fast8_t rsv2;
        uint_fast8_t rsv3;
        std::vector<std::byte> payload;
    };

    class websocket : public inet_socket
    {
        public:
            websocket();
            websocket(http::http& hs_request, int fd);
            int read();
            const std::vector<std::byte>& message() const noexcept { return this->_message; }

            const ws_frame& frame() const noexcept { return this->_frame; };

        private:
            int read_frame();
            
            ws_frame _frame;
            std::vector<std::byte> _message = {};
            int _message_read_successfully = 0;
    };

}

#endif // __DSER_WEBSOCKET_H__

