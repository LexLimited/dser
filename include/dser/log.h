#ifndef __DSER_LOG_H__
#define __DSER_LOG_H__

#include <cstdlib>
#include <concepts>
#include <format>
#include <iostream>
#include <ostream>
#include <source_location>
#include <string_view>

#include <dser/generics.h>

namespace dser::log
{

    namespace
    {

        template<typename T>
        concept printable = requires(T t, std::ostream& os)
        {
            os << t;
        };

        template<typename T>
        concept iterable = requires(T t)
        {
            {  t.begin() };
            { t.end() };
        };

        template<printable T>
        static void print_(T t)
        {
            std::cout << t;
        }

        template<printable T, printable ... Args>
        static void print_(T t, Args ... args)
        {
            std::cout << t << " ";
            print_(args...);
        }

        class debug_base
        {
            public:
                debug_base() : _os(std::cout)
            {}

                debug_base(const std::source_location& loca)
                    : _loca(loca), _os(std::cout)
                {}

            protected:
                std::source_location _loca;
                [[maybe_unused]] std::ostream& _os;
        };
    
    }

    template<printable ... Args>
    void println(Args ... args)
    {
        print_(args...);
        std::cout << std::endl;
    }

    template<iterable Arr>
    void print_container(Arr arr, const std::string_view& delim = ", ")
    {
        auto it = arr.begin();
        auto end = arr.end();
        if (it == end) return;
        
        auto last = std::prev(end);

        std::cout << '(';
        while (it < last)
        {
            std::cout << *it << delim;
            ++it;
        }
        std::cout << *last << ')';
    }

    /// A class for logging in debug mode
    template<printable ... Args>
    class debug : protected debug_base
    {
#if not defined(NDEBUG)
        public:
            debug(Args ... args) : debug_base(std::source_location::current())
            {
                _os << std::format("[LOG] In: file `{} ({}:{})`, function `{}`: ",
                        _loca.file_name(),
                        _loca.line(),
                        _loca.column(),
                        _loca.function_name());
                print_(args...);
                _os << std::endl;
            }

            void set_out_file()
            {}
#else // if defined(NDEBUG)
        public:
            debug(Args ...) {}
#endif
    };

    /// Will log warnings and errors in debug mode (if not defined(NDEBUG))
    /// The output is colored by default, which can be turned off
    /// with `dser::log::debug<>::set_color_output(false)`
    template<>
    class debug<>
    {
        private:
            static bool _color_output;

        public:
            static void set_color_output(bool value)
            {
                debug::_color_output = value;
            }
#if not defined(NDEBUG)
        public:
            debug()
                : _loca(std::source_location::current()),
                  _os(std::cout)
            {}

            template<printable ... Args>
            void warn(Args ... args_)
            {
                this->_severity = LogSeverity::WARNING;
                if (_color_output)
                    _os << ASCII_EscapeSequences::YELLOW;
                this->log(args_...);
            }
            
            template<printable ... Args>
            void error(Args ... args_)
            {
                this->_severity = LogSeverity::ERROR;
                if (_color_output)
                    _os << ASCII_EscapeSequences::RED;
                this->log(args_...);
            }

            template<printable ... Args>
            void fatal(Args ... args_)
            {
                this->_severity = LogSeverity::FATAL;
                if (_color_output)
                    _os << ASCII_EscapeSequences::RED;
                this->log(args_...);
                std::abort();
            }
            
            template<printable ... Args>
            void log(Args ... args_)
            {
                _os << this->get_severity_prefix() << ": ";
                _os << std::format("In: file `{} ({}:{})`, function `{}`: ",
                        _loca.file_name(),
                        _loca.line(),
                        _loca.column(),
                        _loca.function_name());
                print_(args_...);
                if (_color_output)
                    _os << ASCII_EscapeSequences::NO_COLOR;
                _os << std::endl;
            }

        private:
            enum class LogSeverity
            {
                INFO,
                WARNING,
                ERROR,
                FATAL,
            };

            struct ASCII_EscapeSequences
            {
                static constexpr const char* YELLOW="\033[0;33m";
                static constexpr const char* RED="\033[0;31m";
                static constexpr const char* NO_COLOR="\033[0m";
            };

            constexpr const char* get_severity_prefix() const noexcept
            {
                switch (this->_severity)
                {
                    case LogSeverity::WARNING: return "[WARNING]";
                    case LogSeverity::ERROR: return "[ERROR]";
                    case LogSeverity::FATAL: return "[FATAL ERROR]";
                    default: return "[LOG]";
                };
                return "[LOG]";
            }

            std::source_location _loca;
            LogSeverity _severity = LogSeverity::INFO;
            std::ostream& _os;
#else // if defined(NDEBUG)
        public:
            debug() {}
            
            template<printable ... Args>
            void warn(Args ...) {}
            
            template<printable ... Args>
            void error(Args ...) {}

            template<printable ... Args>
            void fatal(Args ...) {}
            
            template<printable ... Args>
            void log(Args ...) {}
#endif
    };

}

#endif // __DSER_LOG_H__

