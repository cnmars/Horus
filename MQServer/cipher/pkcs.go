package cipher

import (
	"crypto/x509"
	"encoding/base64"
	"io/ioutil"
)

var privateKeyFilename = "key"
var publicKeyFilename = privateKeyFilename + ".pub"

func saveKeys(keys *RSAKeys) {

	// Encode to PEM format
	privateBytes := x509.MarshalPKCS1PrivateKey(keys.Private)
	publicBytes := x509.MarshalPKCS1PublicKey(keys.Public)

	// Encode to base64
	b64PrivKey := base64.RawStdEncoding.EncodeToString(privateBytes)
	b64PubKey := base64.RawStdEncoding.EncodeToString(publicBytes)

	keys.B64PrivKey = b64PrivKey
	keys.B64PubKey = b64PubKey

	// Save to disk
	ioutil.WriteFile(privateKeyFilename, []byte(b64PrivKey), 0677)
	ioutil.WriteFile(publicKeyFilename, []byte(b64PubKey), 0677)
}

func GetPublicKeyFilename() string {
	return publicKeyFilename
}

func GetPrivateKeyFilename() string {
	return privateKeyFilename
}
