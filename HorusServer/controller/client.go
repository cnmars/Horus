package controller

import (
	"bufio"
	"fmt"
	"os"
)

// Commands stores all command pending
var Commands chan string

// CommandSent channel is updated whenever a command is sent to client
var CommandSent = make(chan bool, 1)

// ClientConnected is updated whenever a client connects to server
var ClientConnected = make(chan bool, 1)

func setupCommander() {
	Commands = make(chan string, 1)
}

// ReadCommands function read a command and dispatch to all connected clients
func ReadCommands() {

	// Configure commander
	setupCommander()

	// Wait some client to be connected
	fmt.Println("[INFO] Waiting for clients ...")

	<-ClientConnected

	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Printf("shell> ")
		command, fail := reader.ReadString('\n')
		if fail != nil {
			fmt.Printf("[ERROR] Failed to read command: %v\n", fail)
			continue
		}

		// Remove whitespace
		command = command[:len(command)-2]

		// Insert command to channel
		Commands <- command

		fmt.Println("[INFO] Waiting command to be sent for all clients ...")

		// Wait for command to be sent for all clients
		<-CommandSent
	}
}
