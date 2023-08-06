#include <chrono>
#include <dser/utils.h>
#include <thread>

namespace dser {

    void sleep_ms(long ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}
