package mqtt

import (
	"HorusClient/utils"
	"log"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

// brokerHostname constains DNS name of mqtt broker
const brokerHostname string = "broker.hivemq.com:1883"

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

	log.Printf("[INFO] Connected to %v\n", brokerHostname)
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
	go handleMessage(client, message)
}

func onHeartbeatReceived(client MQTT.Client, message MQTT.Message) {
	go handleMessage(client, message)
}
