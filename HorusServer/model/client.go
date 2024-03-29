package model

import (
	"fmt"
	"log"
	"os"
	"sync"
)

// Client struct represents a remote client
type Client struct {
	// ID an UUID that represents this client
	ID string `json:"client_id"`

	// EncryptionKey contains the symetric key used to decrypt data sent by client
	EncryptionKey []byte `json:"encryption_key"`

	// IV contains the initialization vector used for decryption
	IV []byte `json:"initialization_vector"`

	// CmdTopic topic used to receive commands
	CmdTopic string `json:"command_topic"`

	// OutputTopic topic used to send command responses
	OutputTopic string `json:"output_topic"`

	// BaseTopic topic from which all others are derived
	BaseTopic string `json:"base_topic"`

	// HeartbeatTopic topic used to send heartbeat responses
	HeartbeatTopic string `json:"heartbeat_topic"`

	// Logger used to register client messages
	Logger *log.Logger

	// HandshakeTopic topic used to send handshake
	HandshakeTopic string `json:"handshake_topic"`

	// responsesDelivered counts number of responses delivered
	responsesDelivered int

	// deliveredRequests increases the number of delivered requests
	deliveredRequests int

	// clientMu is the interal mutex
	clientMu *sync.Mutex
}

// ClientTopic struct contains name of topics used to make communications
type ClientTopic struct {
	// ID topic id
	ID int

	// Name contains the name of topic
	Name string
}

// ClientRequest struct contains all valid requests from a client
type ClientRequest struct {
	ID   int
	Name string
}

// Topic ID's
var (
	// TopicIDCmd topic used to send commands
	TopicIDCmd = 1

	// TopicIDOut topic used to receive command responses
	TopicIDOut = 2

	// TopicIDBase topic from which all others are derived
	TopicIDBase = 3

	// TopicIDHb topic used to receive heartbeat responses
	TopicIDHb = 4

	// TopicIDHs topic used to send handshake
	TopicIDHs = 5
)

// Request ID's
var (
	RequestIDHeartbeat         = 10
	RequestIDSaveEncryptionKey = 11
)

var (
	// ClientTopics contains the names of all predefined topics
	ClientTopics = []ClientTopic{
		{ID: TopicIDBase, Name: "horus/cmd"},
		{ID: TopicIDOut, Name: "output"},
		{ID: TopicIDCmd, Name: "command"},
		{ID: TopicIDHb, Name: "hb"},
		{ID: TopicIDHs, Name: "hs"},
	}

	// ClientRequests contains all valid requests that can be sent by client
	ClientRequests = []ClientRequest{
		{ID: RequestIDHeartbeat, Name: "/hb"},
	}
)

// GetTopicNameByID returns the topic name that contains the specified ID
func GetTopicNameByID(topicID int) (name string) {
	for _, t := range ClientTopics {
		if t.ID == topicID {
			name = t.Name
			return
		}
	}

	log.Printf("[ERROR] Failed to find topic with id %v", topicID)
	return
}

// GetRequestNameByID returns the request text with specified ID
func GetRequestNameByID(requestID int) (name string) {
	for _, r := range ClientRequests {
		if r.ID == requestID {
			name = r.Name
			return
		}
	}

	log.Printf("[ERROR] Failed to find request with id %v", requestID)

	return
}

// Setup function configures name of topics used by clients
func (c *Client) Setup() {
	if len(c.ID) > 0 {

		// Setup topic names
		c.BaseTopic = fmt.Sprintf("%s/%s", GetTopicNameByID(TopicIDBase), c.ID)
		c.CmdTopic = fmt.Sprintf("%s/%s", c.BaseTopic, GetTopicNameByID(TopicIDCmd))
		c.OutputTopic = fmt.Sprintf("%s/%s", c.BaseTopic, GetTopicNameByID(TopicIDOut))
		c.HeartbeatTopic = fmt.Sprintf("%s/%s", c.BaseTopic, GetTopicNameByID(TopicIDHb))
		c.responsesDelivered = 0
		c.deliveredRequests = 0

		// Configure logger
		logFilename := fmt.Sprintf("client_%s.log", c.ID)
		fileHandle, err := os.Create(logFilename)
		if err != nil {
			log.Printf("[WARN] Failed to setup log system for %v", c.ID)
		} else {
			c.Logger = log.New(fileHandle, "", log.Ldate|log.Ltime)
			c.Logger.Printf("[INFO] base topic: %v", c.BaseTopic)
			c.Logger.Printf("[INFO] cmd topic: %v", c.CmdTopic)
			c.Logger.Printf("[INFO] output topic: %v", c.OutputTopic)
			c.Logger.Printf("[INFO] hb topic: %v", c.HeartbeatTopic)
		}

		return
	}

	log.Fatalf("[CRITICAL] You are trying to setup a client without ID configured")
}

func (c *Client) IncreaseResponsesDelivered() {
	c.responsesDelivered++
}

func (c *Client) GetDeliveredResponses() int {
	return c.responsesDelivered
}

func (c *Client) IncreaseDeliveredRequests() {
	c.deliveredRequests++
}

func (c *Client) GetDeliveredRequests() int {
	return c.deliveredRequests
}

func (c *Client) LockInstance() {
	c.clientMu.Lock()
}

func (c *Client) UnlockInstance() {
	c.clientMu.Unlock()
}
