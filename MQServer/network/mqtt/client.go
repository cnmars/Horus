package network

import (
	"MQServer/utils"
	"fmt"
	"log"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

// brokerHostname constains DNS name of mqtt broker
const brokerHostname string = "broker.hivemq.com:1883"

const baseTopic string = "mqrat/cmd"

// outputTopic contains the name of subtopic used to receive command responses
const outputTopic string = "output"

// cmdTopic name of subtopic used to send commands
const cmdTopic string = "command"

// handshakeTopic topic used to send public key to clients
const handshakeTopic string = "hs"

// client is a handle to mqtt client
var client MQTT.Client

// qosLevel contains the value used for QoS
var qosLevel byte

// Start function starts the MQTT client with the specified QoS level
func Start(QoS byte) {
	// Update QoS level
	qosLevel = QoS

	// Configure options
	options := MQTT.NewClientOptions()
	options.SetAutoReconnect(true)
	options.AddBroker(brokerHostname)
	options.SetClientID(utils.GenerateUUID())
	options.CleanSession = false
	options.WillRetained = false

	// Start client
	client = MQTT.NewClient(options)

	// Connect to broker
	token := client.Connect()
	waitForToken(token)

	// Make subscriptions
	token = client.Subscribe(fmt.Sprintf("%v/+/+", baseTopic), QoS, onMessageReceived)
	waitForToken(token)

	token = client.Subscribe(fmt.Sprintf("%v/+", baseTopic), QoS, onHeartbeatReceived)
	waitForToken(token)
}

// Stop function stops the MQTT client
func Stop() {
	client.Disconnect(5000)
}

func waitForToken(token MQTT.Token) {
	if token.Wait() && token.Error() != nil {
		log.Fatalf("[FATAL] Failed to wait for token: %v", token.Error())
	}
}

func onMessageReceived(client MQTT.Client, message MQTT.Message) {
	go translateMessage(client, message)
}

func onHeartbeatReceived(client MQTT.Client, message MQTT.Message) {
	go translateMessage(client, message)
}
