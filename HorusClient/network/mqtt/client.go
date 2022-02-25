package mqtt

import (
	"HorusClient/model"
	"HorusClient/utils"
	"log"
	"sync"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

// brokerHostname constains DNS name of mqtt broker
const brokerHostname string = "broker.hivemq.com:1883"

// client is a handle to mqtt client
var client MQTT.Client

// customClient is the client used to internal operations
var customClient *model.Client

// qosLevel contains the value used for QoS
var qos byte = 0

// retained is the flag used to
const retained = false

// Global mutex
var mu sync.Mutex

// Start function starts the MQTT client with the specified QoS level
func Start(QoS byte) {
	// Update QoS level
	qos = QoS

	// Configure options
	options := MQTT.NewClientOptions()
	options.SetAutoReconnect(true)
	options.AddBroker(brokerHostname)
	options.SetClientID(utils.GenerateUUID())
	options.CleanSession = false
	options.WillRetained = false
	options.WillQos = qos

	// Start client
	client = MQTT.NewClient(options)

	// Connect to broker
	token := client.Connect()
	waitForToken(token)

	log.Printf("[INFO] Connected to %v\n", brokerHostname)

	customClient = newClient(options.ClientID)

	// Perform subscriptions
	waitForToken(client.Subscribe(customClient.OutputTopic, qos, onResponseReceived))
	waitForToken(client.Subscribe(customClient.CmdTopic, qos, onCommandReceived))

	// Send handshake request
	sendHandshake()

	executeCommands()
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

func publish(topic, message string) {
	token := client.Publish(topic, qos, retained, message)
	waitForToken(token)
}

func onResponseReceived(client MQTT.Client, message MQTT.Message) {
	go handleMessage(client, message)
}

func onCommandReceived(client MQTT.Client, message MQTT.Message) {
	go handleMessage(client, message)
}
