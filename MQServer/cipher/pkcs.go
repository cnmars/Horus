package cipher

import (
	"crypto/x509"
	"encoding/base64"
	"encoding/pem"
	"io/ioutil"
	"log"
)

var privateKeyFilename = "key"
var publicKeyFilename = privateKeyFilename + ".pub"

func saveKeys(keys *RSAKeys) {

	privateKeyBytes := pem.EncodeToMemory(&pem.Block{
		Type:  "RSA PRIVATE KEY",
		Bytes: x509.MarshalPKCS1PrivateKey(keys.Private),
	})

	publicBlockBytes, fail := x509.MarshalPKIXPublicKey(keys.Public)
	if fail != nil {
		log.Fatalf("[CRITICAL] Failed to marshal public key: %v", fail)
	}

	publicKeyBlock := pem.EncodeToMemory(&pem.Block{
		Type:  "PUBLIC KEY",
		Bytes: publicBlockBytes,
	})

	// Encode to base64
	b64PrivKey := base64.RawStdEncoding.EncodeToString(privateKeyBytes)
	b64PubKey := base64.RawStdEncoding.EncodeToString(publicKeyBlock)

	keys.B64PrivKey = b64PrivKey
	keys.B64PubKey = b64PubKey

	// Save to disk
	ioutil.WriteFile(privateKeyFilename, []byte(privateKeyBytes), 0677)
	ioutil.WriteFile(publicKeyFilename, []byte(publicKeyBlock), 0677)
}

func GetPublicKeyFilename() string {
	return publicKeyFilename
}

func GetPrivateKeyFilename() string {
	return privateKeyFilename
}
