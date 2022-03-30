//go:build windows
// +build windows

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
