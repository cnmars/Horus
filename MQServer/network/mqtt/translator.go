package mqtt

import (
	"fmt"
	"log"
	"strings"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

// TranslatedMessage contains the translated message
type TranslatedMessage struct {
	ClientID  string
	TopicName string
	Subtopic  string
	Payload   string
}

func translateMessage(client MQTT.Client, message MQTT.Message) (translated *TranslatedMessage) {
	var clientID string

	// Check topic name
	topic := message.Topic()
	payload := string(message.Payload())
	levels := strings.Split(topic, "/")

	levelCount := len(levels)
	if levelCount < 3 || levelCount > 4 {
		log.Printf("Ignoring message on topic %v", topic)
		return
	}

	if len(payload) == 0 {
		fmt.Printf("[ERROR] Empty payload received on topic: %v", topic)
		return
	}

	// Extract client ID
	clientID = levels[2]

	// Fill struct with translated data
	translated = &TranslatedMessage{}
	translated.ClientID = clientID
	translated.Payload = payload
	translated.TopicName = topic
	translated.Subtopic = levels[levelCount-1]

	return
}
