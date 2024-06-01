#ifndef __DSER_FILE_H__
#define __DSER_FILE_H__

#include <fstream>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>

namespace dser::fs
{

    class file
    {
        public:
            file();
            file(const std::string_view& path);
            ~file();

            const FILE *stream() const noexcept;
            int fd() const noexcept;
            size_t size() const noexcept;
            const char *data() const noexcept;
            int error() const noexcept;

            int open(const std::string_view& path);

        private:
            FILE *_stream = nullptr;
            int _fd = -1;
            size_t _size = 0;
            char *_data = nullptr;
            int _error = 0;
    };

} // namespace dser::fs

#endif // __DSER_FILE_H__

