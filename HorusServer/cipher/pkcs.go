package cipher

import (
	"crypto/x509"
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

	keys.B64PrivKey = string(privateKeyBytes)
	keys.B64PubKey = string(publicKeyBlock)

	// Save to disk
	ioutil.WriteFile(privateKeyFilename, privateKeyBytes, 0777)
	ioutil.WriteFile(publicKeyFilename, publicKeyBlock, 0777)
}

// GetPublicKeyFilename returns the name of file where public key is saved
func GetPublicKeyFilename() string {
	return publicKeyFilename
}

// GetPrivateKeyFilename returns the name of file where private key is saved
func GetPrivateKeyFilename() string {
	return privateKeyFilename
}
