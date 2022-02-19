package mqtt

import (
	"HorusClient/model"
	"fmt"
)

// sendHandshake sends a handshake request to server
func sendHandshake() {

	if !client.IsConnected() {
		fmt.Printf("[ERROR] client is not connected\n")
		return
	}

	publish(customClient.HandshakeTopic, model.GetRequestNameByID(model.RequestIDHandshake))
}
