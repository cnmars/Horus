package network

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strings"
)

func Loop() {

	var command string

	log.Printf("[INFO] Waiting for new clients")

	for {
		if len(Clients) > 0 {
			break
		}
	}

	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("cmd> ")

		// Read command
		command, _ = reader.ReadString('\n')

		// Send command to client
		if len(command) > 0 && client.IsConnected() {
			for _, cl := range Clients {
				client.Subscribe(fmt.Sprintf("%s/%s/output", baseTopic, cl))
				responseTopic := fmt.Sprintf("%s/%s", baseTopic, cl)
				fmt.Printf("[INFO] Sending command to %v\n", responseTopic)
				waitForToken(client.Publish(responseTopic, qosLevel, true,
					strings.ReplaceAll(command, "\n", "")))
			}
		} else {
			log.Printf("[ERROR] Either: client is not connected or command is wrong")
		}
	}
}
