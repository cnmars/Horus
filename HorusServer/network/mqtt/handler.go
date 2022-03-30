package mqtt

import (
	"HorusServer/cipher"
	"HorusServer/controller"
	"encoding/base64"
	"log"

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

func decodePayload(payload string) (decodedPayload []byte, err error) {

	// Calculates max length of decoded payload
	maxLength := base64.RawStdEncoding.DecodedLen(len(payload))

	log.Printf("[INFO] Maximum size of decoded data: %v bytes for %v", maxLength, len(payload))

	// Allocate memory to store decoded data
	var dest []byte = make([]byte, maxLength)

	for s := range dest {
		dest[s] = 0
	}

	// Decode
	_, err = base64.RawStdEncoding.Decode(dest, []byte(payload))

	return dest, err
}

func decryptPayload(payload []byte) (decryptedData []byte, err error) {
	cipher.Decrypt(payload, decryptedData)
	return
}

func notifyController() {
	controller.ClientConnected <- true
}
