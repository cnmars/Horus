package mqtt

import (
	"HorusClient/model"
	"fmt"
)

const handshakeReq string = "hello world"

func sendHandshake() {

	topicName := fmt.Sprintf("%v/%v/%v", model.GetTopicNameByID(model.TopicIDBase), customClient.ID, model.GetTopicNameByID(model.TopicIDHs))

	// Publish a message into handshake topic to notify server
	token := client.Publish(topicName, 0, false, handshakeReq)

	waitForToken(token)
}
