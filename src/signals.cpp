#include <dser/signals.h>
#include <iostream>

namespace dser
{

    void signals::signal_handler(int sig)
    {
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }

    void signals::handle_signals()
    {
        void(*prev)(int);
        for (int sig : handled_signals)
        {
            prev = std::signal(sig, signal_handler);
            if (prev == SIG_ERR)
            {
                std::cout << "Failed to set signal handler for " << sig << std::endl;
            }
        }
    }

}

