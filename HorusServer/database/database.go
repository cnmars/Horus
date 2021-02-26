package database

import (
	"HorusServer/model"
	"fmt"
	"log"

	"github.com/google/uuid"
)

// Clients map contains all information about a client
var clients map[string]model.Client
var newClients chan model.Client

// Initialize function starts the in memory database
func Initialize(maxClients int) {
	newClients = make(chan model.Client, maxClients)
	clients = make(map[string]model.Client)

	// Use a goroutine to register new clients
	go handleNewClients()

	log.Printf("[INFO] Memory database started")
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
			// Insert new client to database
			_, found := clients[newClient.ID]
			if !found {
				clients[newClient.ID] = newClient
			}

			log.Printf("New client registered: %v", newClient.ID)
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

	client.Logger.Printf("[INFO] Saving encryption key: %v", key)

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

	client.Logger.Printf("[INFO] Saving initialization vector: %v", iv)

	// Update IV
	client.IV = []byte(iv)

	return
}

// DeleteClient deletes the specified client from memory
func DeleteClient(clientID string) (err error) {

	// Delete client
	delete(clients, clientID)

	return
}

// FindClientbyID finds the specified client in database
// if err is nil, then the client could be found on database
func FindClientbyID(clientID string) (c *model.Client, err error) {
	cl, found := clients[clientID]
	if found {
		return &cl, nil
	}

	return nil, fmt.Errorf("Client not found in database: %v", clientID)
}

// GetAllClients returns all registered clients
func GetAllClients() []model.Client {
	var allClients []model.Client

	for _, cl := range clients {
		allClients = append(allClients, cl)
	}

	return allClients
}
