package mqtt

import (
	"HorusServer/database"
	"HorusServer/model"
	"bytes"
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

	// Performs a lock into instance
	client.LockInstance()
	defer client.UnlockInstance()

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
		// 1 - perform hex decoding
		// 2 - perform AES256 decryption

		decryptedData, decodeFail := decodePayload(msg.Payload)
		if decodeFail != nil {
			showClientResponse(msg.Payload, client.Logger)
		}

		// removes padding
		decryptedData = bytes.Trim(decryptedData, "\x00")
		client.Logger.Printf("[OUTPUT] %v", string(decryptedData))
		client.IncreaseResponsesDelivered()
	} else {
		output.Printf("[ERROR] Invalid subtopic detected: %v", msg.Subtopic)
	}

	// Checks if client is active
	if client.GetDeliveredResponses() < client.GetDeliveredRequests() {
		log.Printf("[INFO] Deleting inactive client: %v ...", client.ID)
		database.DeleteClient(client.ID)
	}
}

func showClientResponse(msg string, logger *log.Logger) {
	logger.Printf("Response: %v", msg)
}
