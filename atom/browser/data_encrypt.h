/* Copyright http://www.gokulab.com. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef DATA_ENCRYPT_H
#define DATA_ENCRYPT_H

#include <string>
#include <cassert>
#include <openssl/pem.h>
#include <openssl/rsa.h>

class DataEncrypt {
    public:
        DataEncrypt();
        int LoadPublicKey(const std::string& fpath);
        int LoadPrivateKey(const std::string& fpath);
        int DecryptData(const void* src, size_t src_sz, void* dst, size_t dst_sz) const;

        RSA* GetPublicKey() const;
        RSA* GetPrivateKey() const;

    private:
        RSA* public_key_, *private_key_;
};

#endif // DATA_ENCRYPT_H
