package network

import (
	"MQServer/cipher"
	"log"
	"strings"

	MQTT "github.com/eclipse/paho.mqtt.golang"
	"github.com/google/uuid"
)

// Clients contains the ID's of connected clients
var Clients = make(map[string]string)

var commandProcessed = make(chan bool)

// translateMessage function checks if message is a command sent or a command output
func translateMessage(client MQTT.Client, message MQTT.Message) {
	var remoteClientID string

	// Check topic name
	topic := message.Topic()
	payload := string(message.Payload())
	levels := strings.Split(topic, "/")

	levelCount := len(levels)
	if levelCount < 4 || levelCount > 5 {
		return
	}

	// Extract client ID
	remoteClientID = levels[3]

	// Validate client ID
	_, err := uuid.Parse(remoteClientID)
	if err != nil {
		log.Printf("[ERROR] Invalid client ID detected")
		return
	}

	if levelCount == 4 {
		// Check if is heartbeat payload
		if len(payload) == 3 && payload == "/hb" {
			log.Printf("[INFO] Client connected: " + remoteClientID)

			// Save client ID on memory
			Clients[remoteClientID] = topic
		} else {
			log.Printf("[ERROR] Payload not recognized: %v", payload)
		}
	} else if levelCount == 5 {

		// Extract subtopic
		subTopic := levels[4]

		if subTopic == outputTopic {
			// Decrypt command response
			decryptedPayload, decErr := cipher.Decrypt(payload)
			if decErr == nil {
				log.Printf("[RESPONSE] " + string(decryptedPayload))
			}

			commandProcessed <- true
		}
	} else {
		log.Printf("[WARN] Topic name too big: %s", topic)
	}
}

// getTopicNameByClientID returns the topic name used to communicate with specified client
func getTopicNameByClientID(clientID string) string {
	topic, found := Clients[clientID]
	if found {
		return topic
	}

	return ""
}
