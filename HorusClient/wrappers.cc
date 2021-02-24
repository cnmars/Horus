/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include "wrappers.hpp"
#include "mqtt_client.hpp"
#include "paho/include/MQTTClient.h"

void Wrappers::onLost(void *ctx, char *c) {
    static_cast<MqttClient*>(ctx)->OnConnectionLost(ctx, c);
}

int Wrappers::onMsgReceived(void* context, char* topicName, int topicLen, MQTTClient_message *message) {
    return static_cast<MqttClient*>(context)->OnMessageReceived(context, topicName, topicLen, message);
}

void Wrappers::onDelivered(void* ctx, int delivery_token) {
    static_cast<MqttClient*>(ctx)->OnDelivered(ctx, delivery_token);
}
