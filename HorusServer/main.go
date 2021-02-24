package main

import (
	"HorusServer/cipher"
	"HorusServer/controller"
	"HorusServer/database"
	"HorusServer/network/mqtt"
	"HorusServer/utils"
)

func main() {
	// Ensure that cleanup code will be executed
	defer stop()

	// Start server
	start()
}

func start() {
	var qosLevel byte = 0

	// Generate encryption keys with a 4096 bit key
	cipher.EncryptionKeys = cipher.GenerateKeys(4096)

	// Precompute some values
	cipher.EncryptionKeys.Private.Precompute()

	// Configure logging
	f := utils.SetupLogSystem()

	// Starts the in-memory database
	database.Initialize(30)

	// Start MQTT client
	mqtt.Start(qosLevel)

	// Ensure that log file will be closed
	defer f.Close()

	// Read commands from standard input
	controller.ReadCommands()
}

func stop() {
	// Stop MQTT client
	mqtt.Stop()
}