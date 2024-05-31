#ifndef __DSER_PATH_H__
#define __DSER_PATH_H__

#include <string_view>
#include <string>
#include <vector>

#include "utils.h"

namespace dser
{

    class path
    {
        public:
            path(const std::string_view& str)
            {
                set(str);
            }

            void set(const std::string_view& str)
            {
                _segments = split(str, "/");
            }

            const std::vector<std::string> get_segments() const noexcept
            {
                return _segments;
            }

            const std::string get_segment(size_t i) const
            {
                return _segments[i];
            }

        private:
            std::vector<std::string> _segments;
    };

}

#endif // __DSER_PATH_H__

