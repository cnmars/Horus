//go:build windows
// +build windows

package platform

func GetUserName() string {

	usr, fail := user.Current()
	if fail != nil {
		return settings.EmptyString
	}

	return usr.Name
}
