#ifndef __DSER_CONNECTION_H__
#define __DSER_CONNECTION_H__

#include <cstddef>

namespace dser
{

    class connection
    {
        public:
            connection() = default;
            virtual ~connection() = default;

            virtual void read(char* buffer, size_t n) = 0;
            virtual void write(char* buffer, size_t n) = 0;

        private:
    };

}

#endif // __DSER_CONNECTION_H__

