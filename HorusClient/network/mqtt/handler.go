package mqtt

import (
	"log"

	model "HorusClient/model"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

func handleMessage(client MQTT.Client, message MQTT.Message) {
	mu.Lock()
	defer mu.Unlock()

	msg := translateMessage(client, message)
	if msg == nil {
		log.Printf("[ERROR] Cloud not translate message")
		return
	}

	switch msg.Subtopic {
	case model.GetTopicNameByID(model.TopicIDCmd):
		log.Printf("[INFO] Received command: %v", msg.Payload)

		// Sends command to command processor
		CommandsChannel <- msg.Payload
	case model.GetTopicNameByID(model.TopicIDOut):
		log.Printf("[INFO] Output: %v", msg.Payload)
	}
}
