#ifndef __DSER_SOCKET_H__
#define __DSER_SOCKET_H__

namespace dser {

    int open_inet6_socket(int family);
    int bind_inet_socket(const char* domain, const char* port);

    class socket {
        public:
            virtual int open() = 0;
            virtual int close() = 0;
            virtual int listen() = 0;
            virtual int connect(const char* node, const char* servicef) = 0;
            virtual int accept() = 0;
    };

}

#endif // __DSER_SOCKET_H__

