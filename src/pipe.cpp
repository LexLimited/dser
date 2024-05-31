#include <cerrno>
#include <cstdio>
#include <cstring>

#include <dser/pipe.h>
#include <dser/exception.h>
#include <dser/log.h>

namespace dser
{
    
    constexpr const char* pipe::type_to_ptr(pipe_type type)
    {
        switch (type)
        {
            case pipe::pipe_type::READ: return "r";
            case pipe::pipe_type::WRITE: return "w";
        };

        throw dser::exception("Unhandled pipe type");
    }

    pipe::pipe(pipe_type type)
    {
        this->_type = type_to_ptr(type);
    }

    pipe::~pipe()
    {
        this->close();
    }

    int pipe::exec(const std::string_view& cmd)
    {
        this->_file = ::popen(cmd.data(), this->_type);
        if (!this->_file)
        {
            dser::log::debug("Failed to open a pipe", strerror(errno));
            return errno;
        }

        return 0;
    }

    pipe::result pipe::exec_complete(const std::string_view& cmd)
    {
        if (this->exec(cmd))
        {
            throw dser::exception();
        }

        std::string output = this->read_to_string();
        int err = this->close();

        return {err, output};
    }
    
    int pipe::close()
    {
        if (!this->_file) return 0;
        int err = ::pclose(this->_file);
        this->_file = nullptr;
        return err;
    }

    int pipe::fd() const
    {
        if (!this->_file)
        {
            dser::log::debug("No pipe open");
            return -1;
        }

        return ::fileno(this->_file);
    }

    std::string pipe::read_to_string() const
    {
        if (!this->_file)
        {
            dser::log::debug("No pipe open");
            return {};
        }

        const size_t buffer_size = 128;
        std::string buffer;
        std::string result;
        buffer.resize(buffer_size);

        const size_t max_iter = 1000;
        size_t iter = 0;
        while (fgets(buffer.data(), buffer_size, this->_file) && iter++ < max_iter)
        {
            result += buffer;
            std::memset(buffer.data(), 0, buffer_size);
        }
    
        return result;
    }

}

