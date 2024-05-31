#ifndef __DSER_SERVER_H__
#define __DSER_SERVER_H__

#include "inet_socket.h"
#include "router.h"
#include <memory>

namespace dser::http {

    class server
    {
        public:
            int serve(std::string_view port, const std::shared_ptr<dser::router> &r);

        private:
            inet_socket _socket;
            std::shared_ptr<dser::router> _r;
    };

}

#endif // __DSER_SERVER_H__

