package utils

// Find function searches for a in array b. Returns true if a can be found
func Find(a string, b []string) bool {
	for _, value := range b {
		if value == a {
			return true
		}
	}

	return false
}
