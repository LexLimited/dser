#include <cstdio>
#include <cstring>
#include <dser/assert.h>

namespace dser {

    void assert_perr(int success, const char* msg) {
        if (!success) {
            if (msg) perror(msg);
            else perror("assert_perr");
            std::abort();
        }
    }

}
