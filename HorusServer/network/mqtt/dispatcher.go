package mqtt

import (
	"HorusServer/database"
	"HorusServer/model"
	"log"
)

// dispatchMessage function dispatches the specified MQTT message
func dispatchMessage(msg *TranslatedMessage) {

	// Registers the specified client in database
	client, err := database.RegisterClient(msg.ClientID)
	if err != nil {
		log.Printf("[ERROR] Failed to register client: %v", err)
		return
	}

	if msg.TopicName == client.CmdTopic {
		// Just ignore echoed messages
		return
	}

	output := client.Logger

	// Check the message type
	if msg.Subtopic == model.GetTopicNameByID(model.TopicIDHs) {
		output.Printf("[INFO] Handshake received: %v", msg.Payload)
	} else if msg.Subtopic == model.GetTopicNameByID(model.TopicIDOut) {

		// Decode response
		// 1 - perform base64 decoding
		// 2 - perform AES256 decryption

		decodedPayload, decodeFail := decodePayload(msg.Payload)
		if decodeFail != nil {
			// Payload is not base64 encoded. Just show it on screen
			output.Printf("[ERROR] Failed to decode payload: %v", decodeFail)
			showClientResponse(msg.Payload, client.Logger)
		} else {
			// Decrypt
			decryptedData, decryptErr := decryptPayload(decodedPayload)
			if decryptErr != nil {
				output.Printf("[ERROR] Failed to decrypt payload: %v", decryptErr)
				output.Printf("[INFO] Encrypted payload: %v", decodedPayload)
				return
			}

			client.Logger.Printf("[OUTPUT] %v", string(decryptedData))
		}
	} else {
		output.Printf("[ERROR] Invalid subtopic detected: %v", msg.Subtopic)
	}
}

func showClientResponse(msg string, logger *log.Logger) {
	logger.Printf("Response: %v", msg)
}
