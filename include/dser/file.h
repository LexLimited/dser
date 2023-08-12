#ifndef __DSER_FILE_H__
#define __DSER_FILE_H__

#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>

namespace dser::fs
{

    class file
    {
        public:
            file();
            ~file();

            const FILE *stream() const noexcept;
            int fd() const noexcept;
            size_t size() const noexcept;
            const char *data() const noexcept;

            int open(const char *path);

        private:
            FILE *_stream;
            int _fd;
            size_t _size;
            char *_data;
    };

} // namespace dser::fs

#endif // __DSER_FILE_H__

