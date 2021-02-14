/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <memory>

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
             * @return void* Pointer to RSA struct
             */
            void *LoadPublicKey(const char *buffer, size_t len);

            /**
             * @brief Encrypts the specified block of data
             * 
             * @param buffer 
             * @param len
             * @return const char* Base64 encrypted string
             */
            const char *Encrypt(void *buffer, size_t len);

            /**
             * @brief 
             * 
             * @return void* 
             */
            void *GetRSAContext();
        private:
            void *rsa_ctx;
    };
}

#endif
