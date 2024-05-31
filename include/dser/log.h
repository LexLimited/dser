#ifndef __DSER_LOG_H__
#define __DSER_LOG_H__

#include <iostream>
#include <string_view>

namespace dser::log
{

    template<typename T>
    static void print_(T t)
    {
        std::cout << t;
    }

    template<typename T, typename ... Args>
    static void print_(T t, Args ... args)
    {
        std::cout << t << " ";
        print_(args...);
    }

    template<typename ... Args>
    static void debug(Args ... args)
    {
#ifndef NDEBUG
        print_(args...);
        std::cout << std::endl;
#else
#endif
    }

}

#endif // __DSER_LOG_H__

