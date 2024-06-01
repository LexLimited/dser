#ifndef __DSER_LOG_H__
#define __DSER_LOG_H__

#include "dser/generics.h"
#include <cstdlib>
#include <format>
#include <iostream>
#include <source_location>
#include <string_view>

namespace dser::log
{

    namespace {

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

    }

    template<typename ... Args>
    void println(Args ... args)
    {
        print_(args...);
        std::cout << std::endl;
    }

    /// A class for logging in debug mode
    template<typename ... Args>
    class debug
    {
#if not defined(NDEBUG)
        public:
            debug(Args ... args)
                : _loca(std::source_location::current())
            {
                std::cout << std::format("[LOG] In: file `{} ({}:{})`, function `{}`: ",
                        _loca.file_name(),
                        _loca.line(),
                        _loca.column(),
                        _loca.function_name());
                print_(args...);
                std::cout << std::endl;
            }

        private:
            std::source_location _loca;
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
                : _loca(std::source_location::current())
            {}

            template<typename ... Args>
            void warn(Args ... args_)
            {
                this->_severity = LogSeverity::WARNING;
                if (_color_output)
                    std::cout << ASCII_EscapeSequences::YELLOW;
                this->log(args_...);
            }
            
            template<typename ... Args>
            void error(Args ... args_)
            {
                this->_severity = LogSeverity::ERROR;
                if (_color_output)
                    std::cout << ASCII_EscapeSequences::RED;
                this->log(args_...);
            }

            template<typename ... Args>
            void fatal(Args ... args_)
            {
                this->_severity = LogSeverity::FATAL;
                if (_color_output)
                    std::cout << ASCII_EscapeSequences::RED;
                this->log(args_...);
                std::abort();
            }
            
            template<typename ... Args>
            void log(Args ... args_)
            {
                std::cout << this->get_severity_prefix() << ": ";
                std::cout << std::format("In: file `{} ({}:{})`, function `{}`: ",
                        _loca.file_name(),
                        _loca.line(),
                        _loca.column(),
                        _loca.function_name());
                print_(args_...);
                if (_color_output)
                    std::cout << ASCII_EscapeSequences::NO_COLOR;
                std::cout << std::endl;
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
#else // if defined(NDEBUG)
        public:
            debug() {}
            
            template<typename ... Args>
            void warn(Args ...) {}
            
            template<typename ... Args>
            void error(Args ...) {}

            template<typename ... Args>
            void fatal(Args ...) {}
            
            template<typename ... Args>
            void log(Args ...) {}
#endif
    };

}

#endif // __DSER_LOG_H__

