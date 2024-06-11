#include <format>
#include <string_view>
#include <sys/stat.h>
#include <vector>
#include <dirent.h>

#include "utils.h"
#include "pipe.h"
#include "file.h"
#include "log.h"

namespace dser::fs
{

    class directory
    {
        public:
            class item
            {
                public:
                    enum class file_type
                    {
                        UNKNOWN,
                        REGULAR_FILE,
                        DIRECTORY,
                        FIFO,
                        SOCKET,
                        CHAR_DEVICE,
                        BLOCK_DEVICE,
                        SYM_LINK,
                    };

                public:
                    item() = default;
                    item(::dirent* ent, DIR* dir_stream);

                    const char* name() const noexcept;
                    file_type type() const noexcept;
                    dser::result<struct ::stat, int> stats();

                    bool operator==(const item& other) const;
                    bool operator!=(const item& other) const;

                private:
                    file_type dirent_d_type_to_file_type(int d_type) const noexcept;

                private:
                    ::dirent* _ent;
                    DIR* _dir_stream;
            };

            class Iterator
            {
                public:
                    using value_type = item;
                    using reference = item&;
                    using pointer = item*;
                    using iterator_category = std::input_iterator_tag;
                    using difference_type = std::ptrdiff_t;

                    Iterator(DIR* stream);
                    Iterator(DIR* stream, ::dirent* _ent);

                    Iterator& operator++();
                    bool operator==(const Iterator& other) const;
                    bool operator!=(const Iterator& other) const;
                    reference operator*();

                private:
                    DIR* _stream;
                    value_type _item;
            };

        public:
            directory() = default;
            directory(const std::string_view& path);

            /// Returns 0 on success, something else otherwise
            int open(const std::string_view path);

            /// Returns 0 if no stream is open or
            /// it was open and closed uccesfully
            int close();
            bool closed() const;
            int fd() const;
            Iterator begin() const;
            Iterator end() const;

        private:
            DIR* _stream = nullptr;
    };

    /// Lists files and directories
    dser::result<std::vector<std::string>, dser::error> ls(const std::string_view& path);

}
