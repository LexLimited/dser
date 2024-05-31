#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>

#include <dser/utils.h>

namespace dser {

    void sleep_ms(long ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    
    std::string_view trim(const std::string_view sv)
    {
        auto l = sv.begin();
        auto r = std::prev(sv.end());
        
        while(*l == ' ' && l < r) ++l;
        while(*r == ' ' && r > l) --r;

        if (l == sv.end() || *l == ' ') return {};
        return { l, std::next(r) };
    }
    
    std::string_view trim(const std::string& s)
    {
        return trim(std::string_view(s));
    }
    
    std::vector<std::string> split(const std::string_view sv, const std::string_view delim)
    {
        auto l = sv.begin();
        auto r = sv.begin();
        std::string_view rem;
        size_t len;
        std::vector<std::string> res;

        while (l < sv.end())
        {
            rem = std::string_view { l, sv.end() };
            len = rem.find(delim);
            
            if (len == std::string_view::npos)
            {
                res.push_back({ l, sv.end() });
                return res;
            }
            r = l + len;
            if (r > sv.end())
            {
                res.push_back({ l, sv.end() });
                return res;
            }

            if (r > l) res.push_back({ l, r });
            l = r + delim.size();
        }

        return res;
    }
   
    std::pair<std::string_view, std::string_view> split_first(const std::string_view sv, const std::string_view delim)
    {
        size_t offset = sv.find(delim);
        if (offset == std::string_view::npos) return { sv, {} };
        
        std::pair<std::string_view, std::string_view> res;
        res.first = { sv.begin(), sv.begin() + offset };
        if (sv.begin() + offset + 1 == sv.end()) return res;
        res.second = { sv.begin() + offset + 1, sv.end() };
        return res;
    }

    bool in_string(const std::string_view &sv, char c)
    {
        for (char _c : sv)
        {
            if (_c == c) return 1;
        }
        return 0;
    }

    bool in_string(const std::string_view &&sv, char c)
    {
        return in_string(sv, c);
    }

    std::string_view trim_any(const std::string_view sv, std::string_view chars)
    {
        auto l = sv.begin();
        auto r = std::prev(sv.end());

        while (l < r && in_string(chars, *l)) ++l;
        if (l == sv.end()) return {};

        while (r > l && in_string(chars, *r))
        {
            printf("r is in chars: %d\n", *r);
            --r;
        }

        if (in_string(chars, *l)) return {};
        return { l, r == sv.end() ? r : std::next(r) };
    }

    std::string to_lower_case(const std::string_view sv)
    {
        std::string out;
        out.resize(sv.size());
        std::transform(sv.begin(), sv.end(), out.begin(), [](char c){ return std::tolower(c); });

        return out;
    }
    
    std::chrono::time_point<std::chrono::high_resolution_clock> clock_time()
    {
        return std::chrono::high_resolution_clock::now();
    }

    long measure_time(std::chrono::time_point<std::chrono::high_resolution_clock> &last)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock_time() - last).count();
    }
}

