package cipher

import (
	"HorusClient/settings"
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"fmt"
	"log"
)

// keySize contains de size of cipher key (default: 32 to use AES256)
const keySize int = 32

type AESContext struct {
	crypter   cipher.BlockMode
	decrypter cipher.BlockMode
}

func (c *AESContext) GetEncrypter() cipher.BlockMode {
	return c.crypter
}

func (c *AESContext) GetDecrypter() cipher.BlockMode {
	return c.decrypter
}

func (c *AESContext) SetDecrypter(sd cipher.BlockMode) {
	c.decrypter = sd
}

func (c *AESContext) SetEncrypter(se cipher.BlockMode) {
	c.crypter = se
}

var context *AESContext

func SetupAES() {
	// Setup AES cipher
	setupCipher()
}

func setupCipher() {

	var err error

	key := settings.GetConfig().Key
	if len(key) != keySize {
		log.Fatalf("[ERROR] Invalid key length: %v instead of %v", len(key), keySize)
	}

	block, err := aes.NewCipher([]byte(key))
	if err != nil {
		log.Fatalf("[ERROR] Failed to setup AES")
	}

	context = &AESContext{
		crypter:   cipher.NewCBCEncrypter(block, []byte(settings.GetConfig().IV)),
		decrypter: cipher.NewCBCDecrypter(block, []byte(settings.GetConfig().IV)),
	}

	log.Printf("[INFO] Initialized AES cipher with block size %v", context.GetEncrypter().BlockSize())
}

func PKCS7Padding(src []byte, blockSize int) []byte {

	padding := blockSize - len(src)%blockSize
	padtext := bytes.Repeat([]byte{byte(padding)}, padding)
	return append(src, padtext...)
}

// Encrypt encrypts data with AES256
func Encrypt(src []byte) ([]byte, error) {

	if context == nil {
		return nil, fmt.Errorf("cipher not initialized")
	}

	cipherText := PKCS7Padding(src, context.GetEncrypter().BlockSize())
	dest := make([]byte, len(cipherText))
	context.GetEncrypter().CryptBlocks(dest, cipherText)

	return dest, nil
}

// Decrypt decrypts AES256 encrypted data
func Decrypt(src []byte) ([]byte, error) {
	if context == nil {
		return nil, fmt.Errorf("cipher not initialized")
	}

	dest := make([]byte, len(src))
	context.GetDecrypter().CryptBlocks(dest, src)

	return dest, nil
}
