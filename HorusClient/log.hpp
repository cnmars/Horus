/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef MQRAT_LOG_HPP
#define MQRAT_LOG_HPP

#include <string>
#include <cstdarg>

using namespace std;

class Log {
    public:
        /**
         * @brief Print a message to console and exit the program
         * 
         * @param message Message to be printed
         */
        static void LogPanic(string message);

        /**
         * @brief Print a message to console preceeded by a error string
         * 
         * @param message Message to be printed
         */
        static void LogError(string message);

        /**
         * @brief Print a message to console
         * 
         * @param message Message to be printed
         */
        static void LogInfo(string message);

        /**
         * @brief 
         * 
         * @param message 
         * @param ... 
         */
        static void LogInfo(const char *message, ...);

        /**
         * @brief 
         * 
         * @param message 
         */
        static void LogError(const char *message, ...);

        /**
         * @brief 
         * 
         * @param message 
         */
        static void LogPanic(const char *message, ...);
};

#endif
