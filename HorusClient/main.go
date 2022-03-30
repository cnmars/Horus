package main

import (
	"HorusClient/cipher"
	MQTTClient "HorusClient/network/mqtt"
	"HorusClient/platform"
	"HorusClient/settings"
	"fmt"
)

func main() {

	fmt.Printf("%v", platform.GetUserName())
	return

	// Load client settings
	settings.Load()

	// Configure cipher
	cipher.SetupAES()

	// Start MQTT
	MQTTClient.Start(0)

	defer MQTTClient.Stop()
}
