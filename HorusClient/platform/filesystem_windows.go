//go:build windows
// +build windows

package platform

import (
	"golang.org/x/sys"
)

func GetUserName() string {

	usr, fail := user.Current()
	if fail != nil {
		return settings.EmptyString
	}

	return usr.Name
}
