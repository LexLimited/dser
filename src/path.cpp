#include <dser/path.h>

namespace dser
{
    
    path::path(const std::string_view& str)
    {
        set(str);
    }

    void path::set(const std::string_view& str)
    {
        _segments = split(str, "/");
    }

    const std::vector<std::string> path::get_segments() const noexcept
    {
        return _segments;
    }

    const std::string path::get_segment(size_t i) const
    {
        return _segments[i];
    }

    size_t path::n_segments() const
    {
        return _segments.size();
    }

}

