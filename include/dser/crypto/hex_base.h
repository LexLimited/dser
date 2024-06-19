#ifndef __DSER_INCLUDE_CRYPTO_HEX_BASE_H__
#define __DSER_INCLUDE_CRYPTO_HEX_BASE_H__

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace dser
{

    constexpr std::array<uint8_t, 2> byte_to_hex(uint8_t* byte);
    std::string bytes_to_hex_string(const uint8_t* bytes, size_t data_len);

}

#endif // __DSER_INCLUDE_CRYPTO_HEX_BASE_H__

