/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <openssl/evp.h>
#include <openssl/pkcs12err.h>
#include "encoders/base64.hpp"
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <fstream>
#include "crypto.hpp"
#include "log.hpp"

Crypto::RSACipher::RSACipher()
{
    ERR_load_crypto_strings();
    ERR_free_strings();

    rsa_ctx = nullptr;
}

Crypto::RSACipher::~RSACipher()
{
#ifdef TEST
    printf("Destroying RSACipher object\n");
#endif
    RSA_free(static_cast<RSA*>(this->rsa_ctx));
}

void *Crypto::RSACipher::LoadPublicKey(const char *buffer, size_t len)
{
    BIO *bio = nullptr;
    RSA *rsa = nullptr;

    fprintf(stdout, "[INFO] Loading public key:\n%s\n", buffer);

    bio = BIO_new(BIO_s_mem());
    if(!bio) {
        throw std::runtime_error("Failed to initialize BIO memory buffer");
    }

    if(!(BIO_write(bio, buffer, len) > 0)) {
        BIO_free(bio);
        throw std::runtime_error("BIO_write failed");
    }

    rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, NULL, NULL);
    if(!rsa) {
        ERR_print_errors_fp(stdout);
        BIO_free(bio);
        throw std::runtime_error("Failed to read bio pubkey");
    }

    // Setup RSA context variable
    this->rsa_ctx = rsa;

    // Free resources
    BIO_free(bio);

    return rsa;
}

char *Crypto::RSACipher::Encrypt(const char *buffer, size_t len)
{
    std::string s_encryptedBuffer;
    unsigned char *encryptedBuffer;

    // Make sure that public key is loaded
    assert(this->rsa_ctx != nullptr);
    assert(buffer != nullptr);
    
    // Allocates memory to store encrypted data
    auto rsa_size = RSA_size(static_cast<RSA*>(this->rsa_ctx));
    encryptedBuffer = new unsigned char[rsa_size];

    // Truncate payload if it's too big
    rsa_size = (len > rsa_size) ? rsa_size : len;

    if(!(rsa_size > 0)) {
        Log::LogError("Invalid RSA length");
        return NULL;
    }

    Log::LogInfo("Encrypting %u bytes of data", rsa_size);

    // Convert RSA pointer
    auto rsa = static_cast<RSA*>(this->rsa_ctx);

    // Encrypt data
    auto encrypted_size = RSA_public_encrypt(rsa_size,
                            (const unsigned char*)buffer,
                            encryptedBuffer,
                            rsa,
                            RSA_PKCS1_OAEP_PADDING);

    // Check for error
    if(encrypted_size > 0) {
        // Encode encrypted data to base64
        s_encryptedBuffer = base64_encode(encryptedBuffer, encrypted_size);
    } else {
        auto err = ERR_error_string(ERR_get_error(), nullptr);
        throw std::runtime_error(std::string(err));
    }

    Log::LogInfo("Encryption done: " + to_string(encrypted_size) + " bytes encrypted");

    // Free used memory
    delete[] encryptedBuffer;

    // Allocate memory to store encrypted buffer
    char *s = (char*)malloc(s_encryptedBuffer.length() + 1);
    if(!s) {
        Log::LogPanic("Failed to allocate memory: %s\n", strerror(errno));
    }

    // Copy encrypted string to buffer
    strncpy(s, s_encryptedBuffer.c_str(), s_encryptedBuffer.length());

    return s;
}

void *Crypto::RSACipher::GetRSAContext()
{
    return this->rsa_ctx;
}
