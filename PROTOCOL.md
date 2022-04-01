# Horus Protocol
This document describes the communication used in Horus RAT

# Topics

* [Connection](#connection)
* [Handshake](#handshake)
* [Handshake acknowledge](#handshake-acknowledge)
* Command receive
* Command response

# Subtitles
- **CLIENT-ID**: an UUID v4 ID used to identify MQTT client

# Connection

The server and client MUST start a TCP unencrypted connection to broker using default methods

# Handshake

Once a connection has been established, the client MUST send a handshake request to the server by using this method:

1 - Generates a random 16 chars message  
2 - Encrypts the message with AES256   
3 - Encodes the encrypted message to hexadecimal format  
4 - Publishes the message on a topic with the specified format: `horus/cmd/<CLIENT-ID>/hs`

# Handshake acknowledge

The server MUST respond to handshake acnowledge by following:

1 - Decodes message from hexadecimal format  
2 - Decrypts message  
3 - Converts message to base64 string  
4 - Encrypts base64 string  
5 - Encodes to hexadecimal format  
6 - Sends the message to the topic with format: `horus/cmd/<CLIENT-ID>/hs/ack`  
7 - Client SHOULD decrypt message and encode to base64. If the decoded base64 message does not match the initial generated message, the client MUST respond to server on topic `horus/cmd/<CLIENT-ID>` with the message `/error`. The server MUST remove that client from memory database if this message has been received on topic

# Command receive

* Topic: `horus/cmd/<CLIENT-ID>/command`

# Command response

* Topic: `horus/cmd/<CLIENT-ID>/output`



 

