package main

import (
	MQTT "MQServer/network/mqtt"
	"MQServer/utils"
)

func main() {
	// Ensure that cleanup code will be executed
	defer stop()

	// Start server
	start()
}

func start() {
	var qosLevel byte = 0

	// Start MQTT client
	MQTT.Start(qosLevel)

	// Configure logging
	f := utils.SetupLogSystem()

	// Ensure that log file will be closed
	defer f.Close()

	// Read commands from standard input
	MQTT.Loop()
}

func stop() {
	// Stop MQTT client
	MQTT.Stop()
}
