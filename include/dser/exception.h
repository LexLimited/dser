#ifndef __DSER_EXCEPTION_H__
#define __DSER_EXCEPTION_H__

#include <exception>
namespace dser {
    
    class exception : public std::exception {
        public:
            exception(const char* msg): _what(msg) {};
            inline const char* what() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW { return this->_what; }

        private:
            const char* _what;
    };
}

#endif // __DSER_EXCEPTION_H__

