package mqtt

import (
	"HorusServer/controller"
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

	if msg.TopicName == client.HandshakeTopic {
		// Just ignore handshake message
		return
	}

	output := client.Logger

	// Check the message type
	if msg.Subtopic == msg.ClientID {
		// Send public key
		if isHandshakeRequest(msg.Payload) {
			sendPublicKey(*client)
		} else {
			output.Printf("[ERROR] Invalid request on topic %v: %v", msg.TopicName, msg.Payload)
		}
	} else if msg.Subtopic == model.GetTopicNameByID(model.TopicIDOut) {
		// Decode response
		decodedPayload, decodeFail := decodePayload(msg.Payload)
		if decodeFail != nil {
			// Payload is not base64 encoded. Just show it on screen
			output.Printf("[ERROR] Failed to decode payload: %v", decodeFail)
			showClientResponse(msg.Payload, client.ID, client.Logger)
		} else {
			// Decrypt response
			decryptedData, decryptErr := decryptPayload(decodedPayload)
			if decryptErr != nil {
				output.Printf("[ERROR] Failed to decrypt payload: %v", decryptErr)
				controller.CommandProcessed <- true
				return
			}

			// handles the decrypted payload
			handleDecryptedPayload(string(decryptedData), client)
		}
	} else {
		output.Printf("[ERROR] Invalid subtopic detected: %v", msg.Subtopic)
	}

	controller.CommandProcessed <- true
}

func isHandshakeRequest(msg string) bool {
	requestName := model.GetRequestNameByID(model.RequestIDHeartbeat)

	return requestName == msg
}

func showClientResponse(msg, clientID string, logger *log.Logger) {
	logger.Printf("Response from %v: %v", clientID, msg)
}
