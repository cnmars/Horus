//go:build linux
// +build linux

package platform

import (
	"HorusClient/settings"
	"os/user"
)

func GetUserName() string {

	usr, fail := user.Current()
	if fail != nil {
		return settings.EmptyString
	}

	return usr.Name
}
