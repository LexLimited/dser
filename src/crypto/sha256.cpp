#include <cassert>
#include <dser/crypto/sha256.h>

namespace dser
{

    int sha256::hash(const std::byte* data, size_t data_len)
    {
        (void)data;
        (void)data_len;

        assert(0 && "TODO: sha256 is not yet implemented");

        return 0;
    }

}
