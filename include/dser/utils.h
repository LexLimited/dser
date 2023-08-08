#ifndef __DSER_UTILS_H__
#define __DSER_UTILS_H__

#include <string>
#include <vector>

namespace dser {

    void sleep_ms(long ms);
    std::string_view trim(const std::string& s);
    std::vector<std::string_view> split(const std::string_view sv, const std::string_view delim);
    std::pair<std::string_view, std::string_view> split_first(const std::string_view sv, const std::string_view delim);
    std::string_view trim(const std::string_view sv);
    bool in_string(const std::string_view &sv, char c);
    bool in_string(const std::string_view &&sv, char c);
    std::string_view trim_any(const std::string_view sv, std::string_view chars);
    std::string to_lower_case(const std::string_view sv);

}

#endif // __DSER_UTILS_H__

