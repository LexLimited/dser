#ifndef __DSER_UTILS_H__
#define __DSER_UTILS_H__

#include <concepts>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
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

    template<typename Cont, typename It>
    Cont collect(const It& begin, const It& end)
    {
        Cont cont;
        for (It it = begin; it != end; ++it)
            cont.push_back(*it);
        return cont;
    }

    namespace
    {

        template<typename T>
        concept well_constructible = requires()
        {
            std::is_copy_constructible<T>() && std::is_default_constructible<T>();
        };

    }

    class error final
    {
        public:
            error() = default;
            
            error(const error& other) : _what(other._what)
            {}

            error(error&& other) : _what(std::move(other._what))
            {}

            error(const std::string_view& description) : _what(description)
            {}

            ~error()
            {}

            const char* what() const noexcept
            {
                return _what.c_str();
            }

        private:
            std::string _what;
    };

    template<well_constructible Val>
    class result_base
    {
        public:
            result_base() : _has_value(false)
            {}

            result_base(const Val& val) : _value(val), _has_value(true)
            {}

            result_base(result_base&& other)
                : _value(std::move(other._value)), _has_value(other._has_value)
            {}

            bool has_value() const noexcept
            {
                return this->_has_value;
            }

            const Val& value() const noexcept
            {
                return this->_value;
            }

            Val& value() noexcept
            {
                return this->_value;
            }

        protected:
            Val _value;
            bool _has_value;
    };

    template<well_constructible Val, well_constructible Err = void>
    class result final : public result_base<Val>
    {
        public:
            static result<Val, Err> error(const Err& err)
            {
                return result<Val, Err>(std::false_type{}, err);
            }

            static result<Val, Err> ok(const Val& val)
            {
                return result<Val, Err>(val);
            }

        private:
            result(std::false_type, const Err& err) : result_base<Val>(), _error(err)
            {}

        public:
            result(const Val& val) : result_base<Val>(val)
            {}

            result(result<Val, Err>&& other)
                : result_base<Val>(std::move(other)),
                  _error(std::move(other._error))
            {}

            const Err& error() const noexcept
            {
                return this->_error;
            }

            Err& error() noexcept
            {
                return this->_error;
            }

        private:
            Err _error;
    };

    template<well_constructible Val>
    class result<Val, void> : public result_base<Val>
    {
        public:
            result() : result_base<Val>()
            {}

            result(const Val& val) : result_base<Val>(val)
            {}
    };

}

#endif // __DSER_UTILS_H__

