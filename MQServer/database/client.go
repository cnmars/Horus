package database

import (
	"MQServer/model"
	"fmt"

	"github.com/google/uuid"
)

// Clients map contains all information about a client
var clients []model.Client
var newClients chan model.Client
var insertCompleted = make(chan bool, 1)

// Initialize function starts the in memory database
func Initialize(maxClients int) {
	clients = make([]model.Client, maxClients)
	newClients = make(chan model.Client, maxClients)

	insertCompleted <- false

	// Use a goroutine to register new clients
	go handleNewClients()
}

// RegisterClient registers a client in database
func RegisterClient(clientID string) (*model.Client, error) {

	client := &model.Client{}

	// Validates the client ID
	_, err := uuid.Parse(clientID)
	if err != nil {
		return nil, err
	}

	// Checks if the specified client already exists
	c, fail := FindClientbyID(clientID)
	if fail == nil {
		return c, nil
	}

	// Update client ID
	client.ID = clientID

	// Setup topic names for the client
	client.SetupClientInformation()

	// Inserts the new client into channel
	newClients <- *client

	return client, nil
}

func handleNewClients() {
	for {
		select {
		case newClient := <-newClients:
			<-insertCompleted
			clients = append(clients, newClient)
			insertCompleted <- true
			newClient.Logger.Printf("New client registered: %v", newClient.ID)
			break
		}
	}
}

// SaveEncryptionKey saves the encryption key used by client to encrypt data
func SaveEncryptionKey(clientID string, key []byte) (err error) {

	client, fail := FindClientbyID(clientID)
	if fail != nil {
		return fail
	}

	client.Logger.Printf("[INFO] Encryption key: %v", key)

	// Set encryption key
	client.EncryptionKey = []byte(key)

	return
}

// SaveIV saves the IV used by client to encrypt data
func SaveIV(clientID string, iv []byte) (err error) {

	client, fail := FindClientbyID(clientID)
	if fail != nil {
		return fail
	}

	client.Logger.Printf("[INFO] Initialization vector: %v", iv)

	// Update IV
	client.IV = []byte(iv)

	return
}

// DeleteClient makes a logical deletion of the specified client in memory
func DeleteClient(clientID string) (err error) {

	client, fail := FindClientbyID(clientID)
	if fail != nil {
		return fail
	}

	// Delete client
	client.Deleted = true

	return
}

// FindClientbyID finds the specified client in database
// if err is nil, then the client could be found on database
func FindClientbyID(clientID string) (c *model.Client, err error) {
	for _, client := range clients {
		if client.ID == clientID {
			c = &client
			return c, nil
		}
	}

	return nil, fmt.Errorf("Client not found in database: %v", clientID)
}

// GetAllClients returns all registered clients
func GetAllClients() []model.Client {
	return clients
}
