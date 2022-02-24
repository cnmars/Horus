package mqtt

import "HorusClient/model"

func newClient(id string) (c *model.Client) {

	client := &model.Client{ID: id}
	client.Setup()

	return client
}
