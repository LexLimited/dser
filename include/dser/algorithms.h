#ifndef __DSER_ALGORITHMS_H__
#define __DSER_ALGORITHMS_H__

#include <cstddef>

namespace dser::algorithms
{

    void apply_xormask(void* mask, void* data, size_t mask_size, size_t data_size);

}

#endif // __DSER_ALGORITHMS_H__

