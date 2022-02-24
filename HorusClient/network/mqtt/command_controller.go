package mqtt

import (
	"HorusClient/model"
	"HorusClient/platform"
	"log"
	"time"
)

var CommandsChannel = make(chan string, 1)

var (
	CmdUnknown     = -1
	CmdGetUserName = 0
)

var validCommands = map[int]string{
	CmdGetUserName: "get-user-name",
}

func translateCommand(command string) int {

	for i, s := range validCommands {
		if s == command {
			return i
		}
	}

	return CmdUnknown
}

func executeCommands() {

	for {
		command := <-CommandsChannel
		commandId := translateCommand(command)

		switch commandId {
		case CmdUnknown:
			log.Printf("[ERROR] Unknown command: %v", command)
		case CmdGetUserName:
			username := platform.GetUserName()
			publish(model.GetTopicNameByID(model.TopicIDOut), username)
		}

		time.Sleep(time.Second)
	}
}
