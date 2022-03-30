package mqtt

import (
	"encoding/base64"
	"fmt"
	"log"

	"HorusClient/cipher"
	model "HorusClient/model"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

func decryptCommand(cmd string) (string, error) {
	var dest, decryptedCommand []byte
	decodedSize := base64.RawStdEncoding.DecodedLen(len(cmd))

	dest = make([]byte, decodedSize)
	_, err := base64.RawStdEncoding.Decode(dest, []byte(cmd))
	if err != nil {
		return "", fmt.Errorf("could not decode command")
	}

	decryptedCommand = make([]byte, len(dest))

	// Perform decryption
	err = cipher.Decrypt(dest, decryptedCommand)

	return string(dest), err
}

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

		// Decrypts command and sends to command processor
		command, fail := decryptCommand(msg.Payload)
		if fail != nil {
			log.Printf("[ERROR] %v", fail)
			return
		}

		CommandsChannel <- command
	case model.GetTopicNameByID(model.TopicIDOut):
		log.Printf("[INFO] Output: %v", msg.Payload)
	}
}
