#ifndef __DSER_EXCEPTION_H__
#define __DSER_EXCEPTION_H__

#include <exception>
#include <source_location>
#include <string>
#include <format>

namespace dser {
    
    class exception : public std::exception {
        public:
            exception(std::string w, std::source_location loca = std::source_location::current())
                :   _what(std::format("file: {}({}:{}) in function: {}: {}",
                            loca.file_name(), loca.line(), loca.column(), loca.function_name(), w))
            {}

        virtual const char* what() const noexcept override { return this->_what.c_str(); }

        private:
            std::string _what;
    };
}

#endif // __DSER_EXCEPTION_H__

