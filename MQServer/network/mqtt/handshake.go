package mqtt

import (
	"MQServer/cipher"
	"MQServer/model"
	"log"
)

// sendPublicKey function sends the server public key the specified client
func sendPublicKey(c model.Client) {

	log.Printf("[INFO] Sending public key to client %v on topic %v", c.ID, c.HandshakeTopic)

	// Encode payload
	payload := cipher.EncryptionKeys.B64PubKey

	// Publish message
	token := client.Publish(c.HandshakeTopic, qosLevel, false, payload)

	// Check for errors
	waitForToken(token)

	log.Printf("[INFO] Public key sent")
}
