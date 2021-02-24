/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef WRAPPERS_HPP
#define WRAPPERS_HPP

#include "paho/include/MQTTClient.h"

namespace Wrappers {
    void onLost(void *ctx, char *c);
    int onMsgReceived(void* context, char* topicName, int topicLen, MQTTClient_message *message);
    void onDelivered(void* ctx, int delivery_token);
}

#endif
