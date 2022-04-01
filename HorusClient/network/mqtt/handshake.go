package mqtt

import (
	"fmt"
)

const handshakeReq string = "/hello"

func sendHandshake() {

	topicName := fmt.Sprintf("%v", customClient.HandshakeTopic)

	// Publish a message into handshake topic to notify server
	token := client.Publish(topicName, 0, false, handshakeReq)

	waitForToken(token)
}
