package mqtt

import (
	"HorusServer/cipher"
	"HorusServer/controller"
	memoryDatabase "HorusServer/database"
	"HorusServer/model"
	"crypto/aes"
	"encoding/base64"
	"encoding/hex"
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
	return cipher.DecryptBytes(payload)
}

// handleDecryptedPayload processes the decrypted payload
func handleDecryptedPayload(payload string, client *model.Client) {

	// Gets the first 3 bytes of payload, when applies
	if len(payload) >= 3 {

		// Check if payload is encoded with hex
		hexBytes, fail := tryHexDecoding(payload)

		if fail != nil {
			// It's a common decrypted payload
		} else {
			// It's a sk payload. We need to parse it
			hexStr := string(hexBytes)

			header := hexStr[:3]

			// Log header
			log.Printf("header: %v", header)

			if header == "/sk" {

				// Validate payload size
				if isValidSkPayloadLen(hexStr) {
					// Extract IV
					iv := hexBytes[3:19]

					// Extract encryption key
					key := hexBytes[19:]

					// Update client information
					memoryDatabase.SaveEncryptionKey(client.ID, key)

					// Save initialization vector
					memoryDatabase.SaveIV(client.ID, iv)
				} else {
					client.Logger.Printf("[ERROR] Invalid SK payload: (%v bytes) %v ", len(hexStr), hexStr)
				}

			} else {
				// Is just a payload response
				client.Logger.Printf("[INFO] Response: %v", payload)
			}
		}
	} else {
		client.Logger.Printf("[INFO] Generic payload: %v", payload)
	}
}

func isValidSkPayloadLen(payload string) (valid bool) {

	request := model.GetRequestNameByID(model.RequestIDSaveEncryptionKey)

	// Valid payload sizes
	fixedSize := len(request) + aes.BlockSize
	skValidPayloadSizes := []int{
		fixedSize + 32,
		fixedSize + 24,
		fixedSize,
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

func tryHexDecoding(s string) (hexBytes []byte, err error) {
	hexBytes, err = hex.DecodeString(s)
	return
}
