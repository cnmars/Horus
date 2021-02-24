/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <memory>
#include <openssl/rsa.h>
#include <openssl/aes.h>

/**
 * @brief The default cryptographic functions namespace
 * 
 */
namespace Crypto 
{
    /**
     * @brief An RSA cipher
     * 
     */
    class RSACipher 
    {
        public:
            /**
             * @brief Construct a new RSACipher object
             * 
             */
            RSACipher();

            /**
             * @brief Destroy the RSACipher object
             * 
             */
            ~RSACipher();

            /**
             * @brief Loads an RSA public key
             * 
             * @param buffer Buffer containing the base64 public key in PEM format
             * @param len Buffer size
             */
            void LoadPublicKey(const char *buffer, size_t len);

            /**
             * @brief Encrypts the specified block of data
             * 
             * @param buffer 
             * @param len
             * @return char* Base64 encrypted string
             */
            char *Encrypt(const char *buffer, size_t len);

            /**
             * @brief 
             * 
             * @return void* 
             */
            RSA *GetRSAContext();

            /**
             * @brief 
             * 
             */
            void ReleaseCipher();
        private:
            RSA *rsa_ctx;
    };
}

#endif
