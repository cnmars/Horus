package cipher

import (
	"crypto/rand"
	"crypto/rsa"
	"log"
)

// RSAKeys contains the public and private keys
type RSAKeys struct {
	Private    *rsa.PrivateKey
	Public     *rsa.PublicKey
	B64PrivKey string
	B64PubKey  string
}

// EncryptionKeys keys used to encrypt/decrypt data
var EncryptionKeys *RSAKeys

// GenerateKeys generates the keypair used to encrypt/decrypt commands
func GenerateKeys(bits int) (keys *RSAKeys) {
	privateKey, err := rsa.GenerateKey(rand.Reader, bits)
	if err != nil {
		log.Fatalf("[CRITICAL] Failed to generate private key: %v", err)
	}

	// Initialize struct
	keys = &RSAKeys{}

	// Fill struct with keys
	keys.Private = privateKey
	keys.Public = &privateKey.PublicKey
	saveKeys(keys)

	return
}

// Encrypt function encrypt the specified data and return the keys used in process
func Encrypt(data string) (encryptedData []byte, err error) {
	// Encrypt data with public key
	encryptedData, err = rsa.EncryptPKCS1v15(rand.Reader, EncryptionKeys.Public, []byte(data))
	if err != nil {
		log.Printf("[ERROR] Failed to encrypt data: %v", err)
		return
	}

	return
}

// Decrypt function decrypts the specified data
func Decrypt(data string) (decryptedData []byte, err error) {
	privateKey := EncryptionKeys.Private
	decryptedData, err = rsa.DecryptPKCS1v15(rand.Reader, privateKey, []byte(data))
	if err != nil {
		log.Printf("[ERROR] Failed to decrypt data: %v", err)
		return
	}

	return
}
