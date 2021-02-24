/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef AES_HPP
#define AES_HPP

#include <string>
#include <openssl/aes.h>

using namespace std;

namespace Crypto
{
    enum EncryptionType 
    {
        AES128,
        AES192,
        AES256
    };

    /**
     * @brief AES Encryption class
     * 
     */
    class AESCipher
    {
        public:
            /**
             * @brief Construct a new AESCipher object
             * 
             */
            AESCipher();

            /**
             * @brief Construct a new AESCipher object
             * 
             * @param type The encryption type (128, 192 or 256)
             */
            explicit AESCipher(EncryptionType type);

            /**
             * @brief Destroy the AESCipher object
             * 
             */
            ~AESCipher();
            
            /**
             * @brief Configure cipher
             * 
             */
            void Setup();

            /**
             * @brief Encrypts the specified string
             * 
             * @param buffer String to encrypt
             * @return unsigned char* Encrypted string
             */
            unsigned char *Encrypt(string buffer);

            /**
             * @brief Gets the initialization vector
             * 
             * @return unsigned char* 
             */
            unsigned char *GetIV();

            /**
             * @brief Get the key used
             * 
             * @return unsigned char* 
             */
            unsigned char *GetKey();

            /**
             * @brief Get the length of key in bytes
             * 
             * @return unsigned int Key length (bytes)
             */
            unsigned int GetKeySize();
        private:
            /**
             * @brief Generates the initialization vector
             * 
             * @return const unsigned char* 
             */
            const unsigned char *GenerateIV();

            /**
             * @brief Generates the key
             * 
             * @return unsigned char* 
             */
            unsigned char *GenerateKey();

            /**
             * @brief Contains the IV
             * 
             */
            unsigned char iv[AES_BLOCK_SIZE];

            /**
             * @brief Contains the generated key
             * 
             */
            unsigned char *key;

            /**
             * @brief Length of key in bytes
             * 
             */
            unsigned int keySize;

            /**
             * @brief Key size in bits
             * 
             */
            int bits;

            /**
             * @brief Handle to AES key
             * 
             */
            AES_KEY aes_key;
    };
}

#endif
