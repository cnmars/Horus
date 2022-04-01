package mqtt

import (
	"HorusServer/cipher"
	"HorusServer/controller"
	memoryDatabase "HorusServer/database"
	"HorusServer/model"
	"HorusServer/utils"
	"encoding/hex"
	"fmt"
	"log"
	"time"

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

	log.Printf("[INFO] Connected to %v", brokerHostname)

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

func encryptCommand(command string) []byte {

	// Encrypt with AES256
	encryptedCommand, _ := cipher.Encrypt([]byte(command))

	// Encode to hex format
	encodedCommand := make([]byte, len(encryptedCommand)*2)
	hex.Encode(encodedCommand, encryptedCommand)

	return encodedCommand
}

func sendCommands() {

	// Wait until at least one client has been registered
	for len(memoryDatabase.GetAllClients()) == 0 {
		time.Sleep(time.Second)
	}

	// Listens to new commands sent by controller
	for {

		cmd := <-controller.Commands

		// Dispatch the new command to all connected clients
		allClients := memoryDatabase.GetAllClients()

		for _, cl := range allClients {
			fmt.Printf("[INFO] Sending command to %v\n", cl.ID)

			// Encode command
			encodedCommand := string(encryptCommand(cmd))

			fmt.Printf("command: %v\n", string(encodedCommand))

			token := client.Publish(cl.CmdTopic, qosLevel, false, encodedCommand)
			waitForToken(token)
		}

		// Notify controller that the command has been sent to all clients
		controller.CommandSent <- true

	}
}
