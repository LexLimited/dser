#ifndef __DSER_INTERRUPTS_H__
#define __DSER_INTERRUPTS_H__

#include <array>
#include <csignal>

namespace dser::signals {

    inline constinit std::array handled_signals { SIGABRT, SIGFPE, SIGILL, SIGSEGV };
    // std::unordered_map<int, void(*)(int)> standard_signal_handlers;

    extern "C" {
        void signal_handler(int);
    }

    void handle_signals();
}

#endif // __DSER_INTERRUPTS_H__

