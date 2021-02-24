/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef KEYMANAGER_HPP
#define KEYMANAGER_HPP

#include <string>

/**
 * @brief The key manager class
 * 
 */
class KeyManager {
    public:
		/**
		 * @brief Construct a new Key Manager object
		 * 
		 */
		KeyManager();

		/**
		 * @brief Saves the public key
		 * 
		 * @param pk Public key
		 * @param len Key length
		 */
        void SavePublicKey(char *pk, size_t len);

		/**
		 * @brief Reads the saved public key from disk (or memory)
		 * 
		 * @return std::string 
		 */
        std::string ReadPublicKey();

		/**
		 * @brief Gets the filename where public key is saved
		 * 
		 * @return std::string Filename
		 */
		std::string GetFilename();
	private:
		/**
		 * @brief Filename used to store key
		 * 
		 */
		std::string filename;

		/**
		 * @brief Public key
		 * 
		 */
		std::string pk;
};

#endif
