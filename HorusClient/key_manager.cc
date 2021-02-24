/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <fstream>
#include <cstring>
#include "encoders/base64.hpp"
#include "key_manager.hpp"

using namespace std;

KeyManager::KeyManager()
{
	this->filename = "public_key.dat";
	this->pk = "";
}

string KeyManager::GetFilename() {
	return this->filename;
}

void KeyManager::SavePublicKey(char *pk, size_t len) {
	ofstream file(this->filename, ios_base::out);

	// Save key
	file.write(pk, len);

	// Close file
	file.close();
}

string KeyManager::ReadPublicKey() {
	static bool cached = false;

	if(!cached) {

		// Open file
		ifstream file(this->filename, ios_base::in);
		
		// Read public key into memory
		file.seekg(0, file.end);
		auto size = file.tellg();
		file.seekg(0, file.beg);

		// Allocate memory
		auto data = new char[size];
		if(!data) {
			throw std::runtime_error("cannot allocate memory to store public key");
		}

		file.read(data, size);

		// Closes file
		file.close();

		pk = string(data);

		// Free memory
		delete[] data;

		// Update flag
		cached = true;
	}
	
	// returns the previously public key stored in memory
	return pk;
}