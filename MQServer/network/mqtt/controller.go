package network

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"time"
)

var subscribedTopics = make(map[string]string)

func Loop() {

	var command string

	log.Printf("[INFO] Waiting for new clients")

	for {
		if len(Clients) > 0 {
			break
		}

		// Wait some time
		time.Sleep(time.Millisecond * 500)
	}

	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("cmd> ")

		// Read command
		command, _ = reader.ReadString('\n')

		// Send command to client
		if len(command) > 0 && client.IsConnected() {
			for _, clientTopic := range Clients {

				// Topic used to receive command responses
				responseTopic := fmt.Sprintf("%s/%s", clientTopic, outputTopic)

				// Topic used to send commands
				requestTopic := fmt.Sprintf("%s/%s", clientTopic, cmdTopic)

				// Check this server has been subscribed to output topic
				if updateSubscribedTopics(requestTopic, responseTopic) {
					//client.Subscribe(responseTopic, qosLevel, onMessageReceived)
				}

				fmt.Printf("[INFO] Sending command to %v\n", requestTopic)

				// Remove leading whitespaces
				command = command[:len(command)-2]

				// Send command to the specified client
				token := client.Publish(requestTopic, qosLevel, true, command)

				// Wait message to be published
				waitForToken(token)
			}
		} else {
			log.Printf("[ERROR] Either: client is not connected or command is wrong")
			log.Printf("[DEBUG] Command: %v", command)
		}

		fmt.Println("[INFO] Waiting for response ...")

		select {
		case <-commandProcessed:
			break
		}

		fmt.Println("[INFO] Response received")
	}
}

func updateSubscribedTopics(requestTopic, responseTopic string) bool {
	// Checks if topic already exists
	_, found := subscribedTopics[responseTopic]
	if !found {
		subscribedTopics[responseTopic] = requestTopic
		return true
	}

	return false
}
