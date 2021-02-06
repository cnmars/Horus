package network

import (
	"fmt"
	"log"
)

func Loop() {

	var command string

	log.Printf("[INFO] Waiting for new clients")

	for {
		if len(Clients) > 0 {
			break
		}
	}

	// Read command
	for {
		fmt.Print("cmd> ")
		_, err := fmt.Scanln(&command)
		if err != nil {
			log.Printf("[INFO] Failed to read line: %v", err)
		} else {
			// Send command to client
			if client.IsConnected() {
				for _, cl := range Clients {
					waitForToken(client.Publish(cl, 1, false, command))
				}
			}
		}
	}
}
