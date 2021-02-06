package network

import (
	"fmt"
	"log"
	"strings"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

// Clients contains the ID's of connected clients
var Clients []string

func translateMessage(client MQTT.Client, message MQTT.Message) {
	var remoteClientID string

	// Check topic name
	topic := message.Topic()
	payload := string(message.Payload())
	levels := strings.Split(topic, "/")

	if len(levels) == 4 {
		// Check if is heartbeat payload
		if len(payload) == 3 && payload == "/hb" {
			// Extract client id
			remoteClientID = levels[3]
			log.Printf("[INFO] Client connected: " + remoteClientID)

			// Save client ID on memory
			Clients = append(Clients, remoteClientID)
		}
	} else if len(levels) == 5 {

		// Process command response
		fmt.Printf(payload)
	} else {
		log.Printf("[WARN] Topic name too big: %v", topic)
	}
}
