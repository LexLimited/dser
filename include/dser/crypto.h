#ifndef __DSER_CRYPTO_H__
#define __DSER_CRYPTO_H__

#include <string>

namespace dser::crypto {

    std::string sha1(std::string_view);
    std::string base64_encode(std::string_view);

}

#endif // __DSER_CRYPTO_H__

