#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>
#include <ranges>

// 2^26

void bit_trick(std::vector<std::byte>& v1, std::vector<std::byte>& v2, uint32_t mask)
{
    uint64_t mask64 = ((uint64_t)mask) << 32 | mask;
    // uint64_t* payload_ptr = (uint64_t*)v1.data();
    
    // uint64_t local_in[64];
    constexpr size_t local_out_size = 4096;
    uint64_t local_out[local_out_size];
    
    for (size_t i = 0; i < v1.size() / (8 * local_out_size); ++i)
    {
        // memcpy(local_in, (uint64_t*)v1.data() + i * 64, 64 * sizeof(uint64_t));
        
        uint64_t* payload_ptr = (uint64_t*)v1.data() + i * local_out_size;
        for (size_t j = 0; j < local_out_size; ++j) {
            local_out[j] = payload_ptr[j] ^ mask64; // local_in[i] ^ mask64;
        }

        memcpy((uint64_t*)v2.data() + i * local_out_size, local_out, local_out_size * sizeof(uint64_t));
    }

    /*
    for (size_t i = 0; i < v1.size() / 8; ++i) {
        // ((uint64_t*)v2.data())[i] = payload_ptr[i] ^ mask64;
        local[i % 64] = payload_ptr[i] & mask64;
    }
    */
}

void range_trick(std::vector<std::byte>& v1, std::vector<std::byte>& v2, uint32_t mask)
{
    constexpr size_t local_out_size = 4096;
    uint64_t local_out[local_out_size];

    const auto transformation = [mask](auto c) -> auto {
        return c ^ mask;
    };

    auto span1 = std::span<uint64_t> { (uint64_t*)v1.data(), v1.size() / 8 };
    auto span2 = std::span<uint64_t> { (uint64_t*)v2.data(), v2.size() / 8 };

    for (auto it = span1.begin(), it2 = span2.begin(); it < span1.end(); it += local_out_size, it2 += local_out_size)
    {
        std::ranges::transform(
                it, it + local_out_size, local_out,
                transformation, {});
        
        std::copy(local_out, local_out + local_out_size, it2);
    }
}

