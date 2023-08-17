#include <dser/server.h>
#include <iostream>

using namespace dser;

int http::server::serve(std::string_view port, const std::shared_ptr<router> &r) {
    int fd = this->_socket.open();
    if (fd < 0) return -1;
    int err = this->_socket.bind(port.data());
    if (err) return err;

    this->_r = r;

    err = this->_socket.listen();
    if (err) return err;
    while(1) {
        fd = this->_socket.accept();
        std::cout << "accepted fd " << fd << std::endl;
    }

    return 0;
}

