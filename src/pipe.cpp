#include <cerrno>
#include <cstdio>
#include <cstring>

#include <dser/pipe.h>
#include <dser/exception.h>
#include <dser/log.h>
#include <type_traits>
#include <unistd.h>

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

    /// Opens a pipe
    int pipe::exec(const std::string_view& cmd)
    {
        this->_file = ::popen(cmd.data(), this->_type);
        if (!this->_file)
        {
            dser::log::debug{}.error("Failed to open a pipe", strerror(errno));
            return errno;
        }

        return 0;
    }

    /// Executes the command and returns an error if either the command
    /// was finished with an error or if failed to read its output.
    /// Otherwise returns succesfully.
    /// error() contains the exit code,
    /// value() contains the output string if was read succesfully
    dser::result<std::string, int> pipe::exec_complete(const std::string_view& cmd)
    {
        if (int err = this->exec(cmd))
            return dser::result<std::string, int>::error(err);

        auto output = this->read_to_string();
        if (output.has_value())
            return output.value();

        return dser::result<std::string, int>::error(0);
    }
    
    /// Closes the pipe is it was opened and not closed
    int pipe::close()
    {
        if (!this->_file) return 0;
        int err = ::pclose(this->_file);
        this->_file = nullptr;
        return err;
    }

    /// Pipe's file descriptor
    int pipe::fd() const
    {
        if (!this->_file)
        {
            dser::log::debug{}.error("No pipe open");
            return -1;
        }

        return ::fileno(this->_file);
    }

    dser::result<std::string> pipe::read_to_string() const
    {
        int fd;
        if ((fd = this->fd()) < 0)
            return {};
            
        std::string result;
        std::string buffer;
        buffer.resize(512);
        while (ssize_t read_status = ::read(fd, buffer.data(), buffer.size()))
        {
            if (read_status < 0)
            {
                dser::log::debug{}.error("Failed to read from the pipe");
                break;
            }
            result.append(buffer.c_str(), read_status);
        }

        return result;
    }

}

