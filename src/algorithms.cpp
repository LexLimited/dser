#include <dser/algorithms.h>

#include <cstdint>

namespace dser
{
    
    void algorithms::apply_xormask(void* mask, void* data, size_t mask_size, size_t data_size)
    {
        for (size_t i = 0; i < data_size; ++i)
        {
            ((char*)data)[i] ^= ((char*)mask)[i % mask_size];
        }
    }

}
