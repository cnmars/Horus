package main

import (
	"HorusClient/cipher"
	MQTTClient "HorusClient/network/mqtt"
	"HorusClient/settings"
)

func main() {

	// Load client settings
	settings.Load()

	// Configure cipher
	cipher.SetupAES()

	// Start MQTT
	MQTTClient.Start(0)

	defer MQTTClient.Stop()
}
