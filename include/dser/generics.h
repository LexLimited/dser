#ifndef __DSER_GENERICS_HPP__
#define __DSER_GENERICS_HPP__

#include <cstddef>

namespace dser::generics
{

    inline consteval size_t get_parameter_pack_size()
    {
        return 0;
    }

    template<typename T>
    size_t consteval get_parameter_pack_size()
    {
        return 1;
    }

    template<typename First, typename Second, typename ... Rest>
    size_t consteval get_parameter_pack_size()
    {
        return 1 + get_parameter_pack_size<Second, Rest...>();
    }

}

#endif // __DSER_GENERICS_HPP__

