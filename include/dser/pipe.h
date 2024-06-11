#ifndef __DSER_PIPE_H__
#define __DSER_PIPE_H__

#include <cstddef>
#include <string>
#include <string_view>

#include <dser/utils.h>

namespace dser
{

    class pipe
    {
        public:
            enum class pipe_type
            {
                READ,
                WRITE,
            };

        private:
            static constexpr const char* type_to_ptr(pipe_type);

        public:
            pipe(pipe_type = pipe_type::READ);
            ~pipe();

            int exec(const std::string_view& cmd);
            dser::result<std::string, int> exec_complete(const std::string_view& cmd);
            int close();
            int fd() const;
            dser::result<std::string> read_to_string() const;

        private:
            FILE* _file;
            const char* _type;
    };

}

#endif // __DSER_PIPE_H__

