/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <iostream>
#include <atomic>
#include "log.hpp"

using namespace std;

#define print_new_line()	printf("\n")

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

void Log::LogInfo(const char *message, ...) {
	va_list list;

	printf("[INFO] ");
	va_start(list, message);
	vfprintf(stdout, message, list);
	va_end(list);
	print_new_line();
}

void Log::LogError(const char *message, ...) {
	va_list list;

	printf("[ERROR] ");
	va_start(list, message);
	vfprintf(stdout, message, list);
	va_end(list);
	print_new_line();
}

void Log::LogPanic(const char *message, ...) {
	va_list list;

	printf("[CRITICAL] ");
	va_start(list, message);
	vfprintf(stdout, message, list);
	va_end(list);
	print_new_line();
}