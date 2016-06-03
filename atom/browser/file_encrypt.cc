#include "atom/browser/file_encrypt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#define minv(x,y) ( (x>y)?y:x)

FileEncrypt::FileEncrypt() {
}

static const char b64_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};

/*
 *  Decode a string to plain text and return it
 *  */
char *b64_decode_ex (char *src, size_t len, size_t *decsize) {
  int i = 0;
  int j = 0;
  int l = 0;
  size_t size = 0;
  char *dec = NULL;
  unsigned char buf[3];
  unsigned char tmp[4];

  // alloc
  dec = (char *) malloc(0);
  if (NULL == dec) { return NULL; }

  // parse until end of source
  while (len--) {
    // break if char is `=' or not base64 char
    if ('=' == src[j]) { break; }
    if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j])) { break; }

    // read up to 4 bytes at a time into `tmp'
    tmp[i++] = src[j++];

    // if 4 bytes read then decode into `buf'
    if (4 == i) {
      // translate values in `tmp' from table
      for (i = 0; i < 4; ++i) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[i] == b64_table[l]) {
            tmp[i] = l;
            break;
          }
        }
      }

      // decode
      buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
      buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
      buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

      // write decoded buffer to `dec'
      dec = (char *) realloc(dec, size + 3);
      for (i = 0; i < 3; ++i) {
        dec[size++] = buf[i];
      }

      // reset
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 4 times
    for (j = i; j < 4; ++j) {
      tmp[j] = '\0';
    }

    // translate remainder
    for (j = 0; j < 4; ++j) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[j] == b64_table[l]) {
            tmp[j] = l;
            break;
          }
        }
    }

    // decode remainder
    buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
    buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
    buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

    // write remainer decoded buffer to `dec'
    dec = (char *) realloc(dec, size + (i - 1));
    for (j = 0; (j < i - 1); ++j) {
      dec[size++] = buf[j];
    }
  }

  // Make sure we have enough space to add '\0' character at end.
  dec = (char *) realloc(dec, size + 1);
  dec[size] = '\0';

  // Return back the size of decoded string if demanded.
  if (decsize != NULL) *decsize = size;

  return dec;
}

int FileEncrypt::LoadPublicKey(const std::string& fpath) {
    return data_encrypt_.LoadPublicKey(fpath);
}


int FileEncrypt::LoadPrivateKey(const std::string& fpath) {
    return data_encrypt_.LoadPrivateKey(fpath);
}

std::string FileEncrypt::DecryptString(const std::string& message) {
    if (!data_encrypt_.GetPublicKey()) {
        return "";
    }
    const int rsa_sz = RSA_size(data_encrypt_.GetPublicKey());
    std::string ret = "";
    size_t size = (rsa_sz + 2 - ((rsa_sz + 2) % 3)) * 4 / 3;
    for (size_t i = 0; i < message.length(); i+= size) {
        std::string tmp = message.substr(i, minv(message.length() - i, size));
        char* to= new char[rsa_sz - 10];
        memset(to, 0, rsa_sz - 10);
        to[rsa_sz - 11] = '\0';
        size_t dec_len;
        char * value = b64_decode_ex((char *)tmp.c_str(), (unsigned int)tmp.length(), &dec_len);
        int decrypt_sz = data_encrypt_.DecryptData(value, dec_len, to, rsa_sz -11);
        if (-1 == decrypt_sz) {
            return "";
        }
        to[decrypt_sz] = '\0';
        std::string result(value, decrypt_sz);
        ret.append(to);
    }
    return ret;
}

bool IsFileEncrypted(const std::string& fpath) {
    return true;
}
