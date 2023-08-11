#include <dser/crypto.h>

#include <gnutls/crypto.h>
#include <openssl/evp.h>

using namespace dser;

std::string crypto::sha1(std::string_view plain) {
    int err;
    std::string digest;
    digest.resize(20);

    gnutls_hash_hd_t handle = nullptr;
    err = gnutls_hash_init(&handle, gnutls_digest_algorithm_t::GNUTLS_DIG_SHA1);
    if (err) {
        return {};
    }
    
    err = gnutls_hash(handle, plain.data(), plain.size());
    if (err) {
        return {};
    }

    gnutls_hash_output(handle, digest.data());
    return digest;
}

std::string crypto::base64_encode(std::string_view plain) {
    gnutls_datum_t result;
    gnutls_datum_t data;
    data.data = (unsigned char*)plain.data();
    data.size = plain.size();
    
    int err = gnutls_base64_encode2(&data, &result);
    if (err) return {};

    // encoded.resize(4 * ((plain.size() / 3) + bool(plain.size() % 3)));
    // EVP_EncodeBlock((unsigned char*)encoded.data(), (unsigned char*)plain.data(), encoded.size());

    return std::string((char*)result.data);
}

