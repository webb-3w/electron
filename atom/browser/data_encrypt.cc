#include "atom/browser/data_encrypt.h"

DataEncrypt::DataEncrypt() {
    public_key_ = private_key_ = NULL;
}


int DataEncrypt::LoadPublicKey(const std::string& fpath) {
    FILE* handle = fopen(fpath.c_str(), "r");
    do {
        if (!handle) {
            break;
        }
        public_key_ = PEM_read_RSA_PUBKEY(handle, NULL, NULL, NULL);
    } while (false);
    if (handle) {
        fclose(handle);
    }
    return public_key_ == NULL;
}


int DataEncrypt::LoadPrivateKey(const std::string& fpath) {
    FILE* handle = fopen(fpath.c_str(), "r");
    do {
        if (!handle) {
            break;
        }
        private_key_ = PEM_read_RSAPrivateKey(handle, NULL, NULL, NULL);
    } while (false);
    if (handle) {
        fclose(handle);
    }
    return private_key_ == NULL;
}


int DataEncrypt::DecryptData(const void* src, size_t src_sz, void* dst, size_t dst_sz) const {
    if (!public_key_) {
        return -1;
    }
    int decrypted_sz = RSA_public_decrypt(src_sz, (const unsigned char*)src,
            (unsigned char*)dst, public_key_,
            RSA_PKCS1_PADDING);
    if (-1 == decrypted_sz) {
        return -1;
    }
    return decrypted_sz;
}

RSA* DataEncrypt::GetPublicKey() const {
    return public_key_;
}
RSA* DataEncrypt::GetPrivateKey() const {
    return private_key_;
}
