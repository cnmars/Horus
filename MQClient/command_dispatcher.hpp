/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef COMMAND_DISPATCHER_HPP
#define COMMAND_DISPATCHER_HPP

#include "command_dispatcher.hpp"
#include "mqtt_client.hpp"

#define MAX_CMD_SIZE    16

/**
 * @brief The command dispatcher entity
 * 
 */
class CommandDispatcher {
    public:
        CommandDispatcher();
        explicit CommandDispatcher(MQTTClient_message *msg, MqttClient *client, char *topic);
        void Dispatch();
        void setMessage(MQTTClient_message *new_msg);
        void setTopic(char *topic_name);
    private:
        MQTTClient_message *message;
        MqttClient *client;
        char *topic;
};

#endif
