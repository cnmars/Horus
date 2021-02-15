package network

import (
	"MQServer/cipher"
	"fmt"
	"log"
)

// sendPublicKey function sends the server public key to it's clients
func sendPublicKey(clientID string) {

	// build topic name
	topic := fmt.Sprintf("%s/%s", Clients[clientID], handshakeTopic)

	log.Printf("[INFO] Sending public key to client %v on topic %v", clientID, topic)

	// Encode payload
	payload := cipher.EncryptionKeys.B64PubKey

	// Publish message
	token := client.Publish(topic, qosLevel, false, payload)

	// Check for errors
	waitForToken(token)

	log.Printf("[INFO] Public key sent: %v", payload)
}
