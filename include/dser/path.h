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
            path(const std::string_view& str);

            void set(const std::string_view& str);
            const std::vector<std::string> get_segments() const noexcept;
            const std::string get_segment(size_t i) const;
            size_t n_segments() const;

        private:
            std::vector<std::string> _segments;
    };

}

#endif // __DSER_PATH_H__

