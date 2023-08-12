#ifndef __DSER_DESKTOP_FILE_H__
#define __DSER_DESKTOP_FILE_H__

#include <string>
#include <unordered_map>
#include <variant>

namespace dser
{

    class desktop_file
    {
        using value = std::variant<std::string, int>;

        public:
            desktop_file() = default;
            ~desktop_file() = default;

            int read(std::string_view path);
 
        private:
            std::unordered_map<std::string, value> values;
    };

}

#endif // __DSER_DESKTOP_FILE_H__

