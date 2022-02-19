package mqtt

import "HorusClient/model"

func newClient() (c *model.Client) {

	client := &model.Client{}
	client.Setup()

	return client
}
