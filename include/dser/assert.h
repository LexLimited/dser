#ifndef __DSER_ASSERT_H__
#define __DSER_ASSERT_H__

#include <assert.h>
#include <cstdio>
#include <cstdlib>

namespace dser
{

    /// Asserts the condition, prints the message and `strerror(errno)`
    void assert_perr(int success, const char* msg = nullptr);

}

#endif // __DSER_ASSERT_H__

