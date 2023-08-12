#include <dser/file.h>
#include <sys/mman.h>

namespace dser::fs
{

    file::file() : _stream(0), _fd(0), _size(0), _data(nullptr) {}

    file::~file()
    {
        if (this->_data)
        {
            munmap(this->_data, this->_size);
        }
    }

    const FILE* file::stream() const noexcept { return this->_stream; }
    int file::fd() const noexcept { return this->_fd; }
    size_t file::size() const noexcept { return this->_size; }
    const char* file::data() const noexcept { return this->_data; }

    int file::open(const char* path)
    {
        this->_stream = std::fopen(path, "r");
        if (!this->_stream) return errno;

        this->_fd = fileno(this->_stream);
        if (!this->_fd)
        {
            std::fclose(this->_stream);
            return errno;
        }

        struct stat f_stat;
        if(fstat(this->_fd, &f_stat))
        {
            std::fclose(this->_stream);
            return errno;
        }
    
        this->_size = f_stat.st_size;
        this->_data = (char*)mmap(
                nullptr, this->_size,
                PROT_READ, MAP_PRIVATE,
                this->_fd, 0);
        std::fclose(this->_stream);
        if (!this->_data) return errno;
        return 0;
    }

}
