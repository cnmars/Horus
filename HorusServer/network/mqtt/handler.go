package mqtt

import (
	"HorusServer/cipher"
	"HorusServer/controller"
	"encoding/hex"

	MQTT "github.com/eclipse/paho.mqtt.golang"
)

func handleMessage(client MQTT.Client, message MQTT.Message) {

	// Translate the message
	tm := translateMessage(client, message)

	// Dispatch the message
	dispatchMessage(tm)

	// Notify controller that we have a new client
	notifyController()
}

// decodePayload decodes a payload from hex format and decrypts it with AES256 algorithm
func decodePayload(payload string) (decodedPayload []byte, err error) {

	// Decode from hex
	encryptedPayload, err := hex.DecodeString(payload)
	if err != nil {
		return
	}

	// Decrypt payload
	decryptedPayload, err := cipher.Decrypt([]byte(encryptedPayload))
	if err != nil {
		return
	}

	return decryptedPayload, nil
}

func decryptPayload(payload []byte) (decryptedData []byte, err error) {
	return cipher.Decrypt(payload)
}

func notifyController() {
	controller.ClientConnected <- true
}
