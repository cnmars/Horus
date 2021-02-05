/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <iostream>
#include "log.hpp"

using namespace std;

void Log::LogPanic(string msg) {
    cout << "[FATAL] " << msg << endl;
    exit(1);
}

void Log::LogError(string msg) {
    cout << "[ERROR] " << msg << endl;
}

void Log::LogInfo(std::string message) {
	cout << "[INFO] " << message << endl;
}
