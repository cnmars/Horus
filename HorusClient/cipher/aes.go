package cipher

import (
	"HorusClient/settings"
	"crypto/aes"
	"crypto/cipher"
	"fmt"
	"log"
)

// keySize contains de size of cipher key (default: 32 to use AES256)
const keySize int = 32

type AESContext struct {
	Block cipher.Block
}

func (c AESContext) GetBlock() cipher.Block {
	return c.Block
}

var context *AESContext

func SetupAES() {
	context = &AESContext{}

	// Setup AES cipher
	getCipherKey()
}

func getCipherKey() {
	var err error

	key := settings.GetConfig().Key
	if len(key) != keySize {
		log.Fatalf("[ERROR] Invalid key length: %v instead of %v", len(key), keySize)
	}

	block, err := aes.NewCipher([]byte(key))
	if err != nil {
		log.Fatalf("[ERROR] Failed to setup AES")
	}

	context.Block = block

	log.Printf("[INFO] Initialized AES cipher with block size %v", context.GetBlock().BlockSize())
}

// Encrypt encrypts data with AES256
func Encrypt(src, dest []byte) (err error) {

	if context == nil {
		return fmt.Errorf("cipher not initialized")
	}

	context.GetBlock().Encrypt(src, dest)

	err = nil
	return
}

// Decrypt decrypts AES256 encrypted data
func Decrypt(src, dest []byte) (err error) {
	if context == nil {
		return fmt.Errorf("cipher not initialized")
	}

	context.GetBlock().Decrypt(src, dest)
	err = nil
	return
}
