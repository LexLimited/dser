#ifndef __DSER_PIPE_H__
#define __DSER_PIPE_H__

#include <cstddef>
#include <string>
#include <string_view>

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

            struct result
            {
                int error;
                std::string output;
            };

        private:
            static constexpr const char* type_to_ptr(pipe_type);

        public:
            pipe(pipe_type);
            ~pipe();

            int exec(const std::string_view& cmd);
            result exec_complete(const std::string_view& cmd);
            int close();
            int fd() const;
            std::string read_to_string() const;

        private:
            FILE* _file;
            const char* _type;
    };

}

#endif // __DSER_PIPE_H__

