/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <windows.h>
#include "utils.hpp"
#include "log.hpp"

void Utils::GenerateRandomSeed() {
#ifdef WIN32
    srand(GetTickCount());
#elif defined(WIN64)
    srand(GetTickCount64());
#endif
}

int Utils::GetRandomPositive() {
    int number;

    do {
        number = rand();
        
        // Gets a new seed
        Utils::GenerateRandomSeed();
    } while(number <= 0);
	
	return number;
}

std::string Utils::GenerateID() {
#ifdef WIN32
    UUID uuid;
    char *uuid_str = NULL;
	
    if(UuidCreate(&uuid) == RPC_S_OK) {
        UuidToStringA(&uuid, (RPC_CSTR*)&uuid_str);
		auto size = std::string(uuid_str).length();
		if(size != 36) {
			Log::LogPanic("Invalid UUID generated: " + std::to_string(size));
		}
		
        return std::string(uuid_str);
    }
    
    return "";
#else
    throw "Not supported";
#endif
}

std::string Utils::ToLower(std::string& s)
{
    for(auto& ch : s) {
        ch = std::tolower(ch);
    }

    return s;
}

vector<string> Utils::Split(string s, char delim)
{
    vector<string> v;
    string elem = "";
    bool in_qm = false;

    for(auto c : s) {
        if(c == '\"') {
            in_qm = !in_qm;
            continue;
        }

        if(in_qm) {
            elem += c;
            continue;
        } else {
            if(c != delim) {
                elem += c;
            } else {
                v.push_back(elem);
                elem.clear();
            }
        }
    }

    // Append the last element to list
    v.push_back(elem);

    return v;
}

char *ToHex(string text)
{
    int size = text.length();
    int limit = (text.length() * 2) + 1;
    char *hexstring = (char*)malloc(sizeof(char) * limit);

    // Initialize memory
    RtlSecureZeroMemory(&hexstring, sizeof(hexstring));

    for(auto i = 0, j = 0; i < size; i++, j += 2) {
        snprintf(&hexstring[j], 3, "%.2x", text[i]);
    }

    return hexstring;
}
