package utils

import (
	"log"
	"os"
)

const logFilename string = "server.log"

// SetupLogSystem function configures the logsystem
func SetupLogSystem() (f *os.File) {
	f, err := os.OpenFile(logFilename, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0666)
	if err != nil {
		log.Fatalf("[FATAL] Failed to open log file: %v", err)
	}

	log.SetOutput(f)
	return
}
