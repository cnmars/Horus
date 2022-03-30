package settings

import (
	"encoding/json"
	"io/ioutil"
	"log"
)

type settings struct {
	Key string `json:"key"`
}

const filename string = "settings.json"

var config settings

func GetConfig() *settings {
	return &config
}

func Load() {

	data, err := ioutil.ReadFile(filename)
	if err != nil {
		log.Fatalf("[ERROR] Could not load settings: %v", err)
	}

	err = json.Unmarshal(data, &config)
	if err != nil {
		log.Fatalf("[ERROR] Could not decode settings file")
	}
}
