package mqtt

import (
	"bytes"
	"encoding/hex"
	"log"
	"strings"

	"HorusClient/cipher"
	model "HorusClient/model"
	"HorusClient/settings"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

func decryptCommand(cmd string) (string, error) {

	decodedCommand, err := hex.DecodeString(cmd)
	if err != nil {
		log.Printf("[ERROR] Could not decode hexadecimal data: %v", err)
		return settings.EmptyString, nil
	}

	// Decrypt command
	decryptedCommand, err := cipher.Decrypt(decodedCommand)
	if err != nil {
		log.Printf("[ERROR] Could not decrypt command: %v", err)
	}

	return strings.TrimSpace(string(decryptedCommand)), nil
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

		CommandsChannel <- string(bytes.Trim([]byte(command), "\x00"))
	case model.GetTopicNameByID(model.TopicIDOut):
		log.Printf("[INFO] Output: %v", msg.Payload)
	}
}
