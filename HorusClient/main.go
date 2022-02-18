package main

import (
	MQTTClient "HorusClient/network/mqtt"
)

func main() {

	MQTTClient.Start(0)

	defer MQTTClient.Stop()
}
