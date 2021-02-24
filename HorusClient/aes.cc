/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <windows.h>
#include <cassert>
#include <exception>
#include "aes.hpp"
#include "log.hpp"

Crypto::AESCipher::AESCipher()
{
    RtlSecureZeroMemory(&iv, sizeof(iv));
    RtlSecureZeroMemory(&key, sizeof(key));
    
    // Default bitsize
    bits = 128;

    keySize = (bits / 8) * sizeof(unsigned char);

    Log::LogInfo("Setting up AES Cipher ...");
}

Crypto::AESCipher::AESCipher(EncryptionType t) : AESCipher()
{   
    switch(t) 
    {
        case AES128: bits = 128; break;
        case AES192: bits = 192; break;
        case AES256: bits = 256; break;
        default: bits = 128; break;
    }

    // allocate memory to store key
    auto mem_to_alloc = (bits / 8) * sizeof(unsigned char);
    key = new unsigned char[mem_to_alloc];

    if(!key) {
        Log::LogPanic("Failed to allocate memory for AES key: %s", strerror(errno));
    }

    // Set key size
    keySize = mem_to_alloc;
}

Crypto::AESCipher::~AESCipher()
{
    free(this->key);
}

void Crypto::AESCipher::Setup()
{
    // Generate key and IV
    GenerateIV();
    GenerateKey();

    // Associate key
    auto status = AES_set_encrypt_key(key, bits, &this->aes_key);
    if(status != 0) {
        Log::LogPanic("Failed to set encryption key: %d", status);
    }
}

unsigned char *Crypto::AESCipher::Encrypt(string buffer)
{
    unsigned char out[AES_BLOCK_SIZE];
    unsigned char in[AES_BLOCK_SIZE];
    size_t str_size;
    unsigned char *full_output = nullptr;
    unsigned int len, bytes_encrypted = 0, index = 0;

    str_size = buffer.length();

    // The minimal size is the block size
    str_size = (str_size < AES_BLOCK_SIZE) ? AES_BLOCK_SIZE : str_size;

    // Allocate memory
    full_output = new unsigned char[sizeof(unsigned char) * str_size];
    
    assert(full_output != nullptr);

    while(bytes_encrypted < str_size)
    {
        // Get chunk of data
        for(len = 0; len < AES_BLOCK_SIZE; len++) {
            in[len] = static_cast<unsigned char>(buffer[bytes_encrypted]);
            bytes_encrypted++;
        }

        // Encrypt payload
        AES_cbc_encrypt((const unsigned char*)in, 
                        (unsigned char*)out, 
                        len, 
                        &aes_key, iv, 1);

        // Put the encrypted buffer into the full output buffer
        memcpy(&full_output[index], out, len);

        index += (bytes_encrypted + 1);
    }

    return full_output;
}

const unsigned char *Crypto::AESCipher::GenerateIV()
{
    static bool have_iv = false;

    srand(GetTickCount64());

    if(!have_iv) {
        for(auto i = 0; i < ARRAYSIZE(this->iv); ++i) {
            this->iv[i] = (const UCHAR)((rand() % 255) + 1);
        }
        have_iv = true;
    }

    return &iv[0];
}

unsigned char *Crypto::AESCipher::GenerateKey()
{
    static bool have_key = false;

    if(!have_key) {
        for(auto i = 0; i < keySize; i++) {
            this->key[i] = (UCHAR)((rand() % 255) + 1);
        }
        have_key = true;
    }

    return &this->key[0];
}

unsigned char *Crypto::AESCipher::GetIV()
{
    return &iv[0];
}

unsigned char *Crypto::AESCipher::GetKey()
{
    return &key[0];
}

unsigned int Crypto::AESCipher::GetKeySize()
{
    return this->keySize;
}
