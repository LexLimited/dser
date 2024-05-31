#ifndef __DSER_UTILS_H__
#define __DSER_UTILS_H__

#include <string>
#include <vector>
#include <chrono>

namespace dser
{

    void sleep_ms(long ms);
    std::string_view trim(const std::string& s);
    std::vector<std::string> split(const std::string_view sv, const std::string_view delim);
    std::pair<std::string_view, std::string_view> split_first(const std::string_view sv, const std::string_view delim);
    std::string_view trim(const std::string_view sv);
    bool in_string(const std::string_view &sv, char c);
    bool in_string(const std::string_view &&sv, char c);
    std::string_view trim_any(const std::string_view sv, std::string_view chars);
    std::string to_lower_case(const std::string_view sv);

    std::chrono::time_point<std::chrono::high_resolution_clock> clock_time();
    long measure_time(std::chrono::time_point<std::chrono::high_resolution_clock> &last);
}

#endif // __DSER_UTILS_H__

