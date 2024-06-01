#include "dser/log.h"
#include <cerrno>
#include <cstring>
#include <sys/mman.h>

#include <dser/file.h>

namespace dser::fs
{

    file::file() : _stream(0), _fd(0), _size(0), _data(nullptr) {}

    file::file(const std::string_view& path)
    {
        this->open(path);
    }
    
    file::~file()
    {
        if (this->_data)
        {
            munmap(this->_data, this->_size);
        }
    }

    const FILE* file::stream() const noexcept
    {
        return this->_stream;
    }
    
    int file::fd() const noexcept
    {
        return this->_fd;
    }
    
    size_t file::size() const noexcept
    {
        return this->_size;
    }
    
    const char* file::data() const noexcept
    {
        return this->_data;
    }

    int file::error() const noexcept
    {
        return this->_error;
    }

    int file::open(const std::string_view& path)
    {
        this->_stream = std::fopen(path.data(), "r");
        if (!this->_stream)
        {
            dser::log::debug{}.error("Failed to open file");
            return this->_error = errno;
        }

        this->_fd = fileno(this->_stream);
        if (!this->_fd)
        {
            dser::log::debug("Failed to get file descriptor");
            std::fclose(this->_stream);
            return this->_error = errno;
        }

        struct stat f_stat;
        if(fstat(this->_fd, &f_stat))
        {
            dser::log::debug("Failed to get file stats");
            std::fclose(this->_stream);
            return this->_error = errno;
        }
    
        this->_size = f_stat.st_size;
        this->_data = (char*)mmap(
                nullptr, this->_size,
                PROT_READ, MAP_PRIVATE,
                this->_fd, 0);

        std::fclose(this->_stream);
        if (!this->_data)
        {
            dser::log::debug("Failed to mmap file");
            return this->_error = errno;
        }

        return 0;
    }

}
