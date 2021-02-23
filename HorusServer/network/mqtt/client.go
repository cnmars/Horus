package mqtt

import (
	"HorusServer/controller"
	memoryDatabase "HorusServer/database"
	"HorusServer/model"
	"HorusServer/utils"
	"fmt"
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

	// Gets the name of base topic
	baseTopic := model.GetTopicNameByID(model.TopicIDBase)

	// Make subscriptions
	token = client.Subscribe(fmt.Sprintf("%v/+/+", baseTopic), QoS, onMessageReceived)
	waitForToken(token)

	token = client.Subscribe(fmt.Sprintf("%v/+", baseTopic), QoS, onHeartbeatReceived)
	waitForToken(token)

	// Trigger the command sender
	go sendCommands()
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

func sendCommands() {
	for {
		select {
		case cmd := <-controller.Commands:

			// Dispatch the new command to all connected clients
			allClients := memoryDatabase.GetAllClients()

			for _, cl := range allClients {
				// Ignore deleted clients
				if !cl.Deleted {
					fmt.Printf("[INFO] Sending command to %v\n", cl.ID)
					client.Publish(cl.CmdTopic, qosLevel, false, cmd)
				}
			}

			break
		}
	}
}
