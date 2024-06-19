#include <cstring>

#include <dser/crypto/hex_base.h>

namespace dser
{

    constexpr std::array<uint8_t[2], 256> get_byte_hex_map()
    {
        const uint8_t hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
        std::array<uint8_t[2], 256> ret{};

        for (size_t i = 0; i < ret.size(); i++)
        {
            ret[i][0] = hex_digits[i >> 4];
            ret[i][1] = hex_digits[i & 0xf];
        }

        return ret;
    }

    std::string bytes_to_hex_string(const uint8_t* bytes, size_t data_len)
    {
        if (!bytes || !data_len) return "";

        std::string ret(2 * data_len, 0);

        auto ret_ptr = reinterpret_cast<uint8_t*>(ret.data());
        auto byte_hex_map = get_byte_hex_map();
        for (size_t i = 0; i < data_len; ++i)
        {
            std::memcpy(ret_ptr, byte_hex_map[bytes[i]], 2);
            ret_ptr += 2;
        }

        return ret;
    }

}
