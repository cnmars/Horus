package utils

import (
	"github.com/google/uuid"
)

// GenerateUUID functions generates a UUID and returns it string representation
func GenerateUUID() string {
	return uuid.New().String()
}
