#ifndef __DSER_INCLUDE_CRYPTO_SHA256_H__
#define __DSER_INCLUDE_CRYPTO_SHA256_H__

#include <cstddef>
#include <string>

namespace dser
{

    class sha256
    {
        public:
            static constexpr size_t OUT_BYTES = 32UL;

            int hash(const std::byte* data, size_t data_len);
            std::string digest();

        private:
            /// Length of a padded message in bytes
            size_t _len{ 0 };
    };

}

#endif // __DSER_INCLUDE_CRYPTO_SHA256_H__
