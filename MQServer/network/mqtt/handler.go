package mqtt

import (
	"MQServer/cipher"
	"MQServer/controller"
	memoryDatabase "MQServer/database"
	"MQServer/model"
	"crypto/aes"
	"encoding/base64"

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

	// Allocate memory to store decoded data
	var dest []byte = make([]byte, maxLength-1)

	// Decode
	_, err = base64.RawStdEncoding.Decode(dest, []byte(payload))

	return dest, err
}

func decryptPayload(payload []byte) (decryptedData []byte, err error) {
	return cipher.DecryptBytes(payload)
}

// handleDecryptedPayload processes the decrypted payload
func handleDecryptedPayload(payload string, client *model.Client) {

	// Gets the first 3 bytes of payload, when applies
	if len(payload) >= 3 {

		payloadBytes := []byte(payload)

		header := payload[:3]
		if header == "/sk" {

			// Validate payload size
			if isValidSkPayloadLen(payload) {
				// Extract IV
				iv := payloadBytes[3:19]

				// Extract encryption key
				key := payloadBytes[:20]

				// Update client information
				memoryDatabase.SaveEncryptionKey(client.ID, key)

				// Save initialization vector
				memoryDatabase.SaveIV(client.ID, iv)
			}

			client.Logger.Printf("[ERROR] Invalid SK payload: %v", payload)
		} else {
			// Is just a payload response
			client.Logger.Printf("[INFO] response from %v: %v", client.ID, payload)
		}
	}
}

func isValidSkPayloadLen(payload string) (valid bool) {

	request := model.GetRequestNameByID(model.RequestIDSaveEncryptionKey)

	// Valid payload sizes
	skValidPayloadSizes := []int{
		len(request) + aes.BlockSize + 32,
		len(request) + aes.BlockSize + 24,
		len(request) + aes.BlockSize,
	}

	valid = false
	payloadLen := len(payload)

	for _, skLen := range skValidPayloadSizes {
		if payloadLen == skLen {
			valid = true
			break
		}
	}

	return
}

func notifyController() {
	controller.ClientConnected <- true
}
