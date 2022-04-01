# Horus
Horus is a remote administration tool that uses a cloud MQTT broker to send and receive commands

For educational purposes only

# Features
- Bypass most firewalls
- Multiplatform client and server
- AES256 encryption

# Usage

### Server
1 - Deploy server

```bash
go get -v
go build
touch settings.json
```

2 - Configure encryption password and IV and save to `settings.json`

```json
{
  "key": "abcd1234abcd1234abcd1234abcd1234",
  "iv": "abcd1234abcd1234"
}
```

### Client

1 - Build client
```bash
go get -v
go build
touch settings.json
```

2 - Configure encryption password and IV and save to `settings.json`

```json
{
  "key": "abcd1234abcd1234abcd1234abcd1234",
  "iv": "abcd1234abcd1234"
}
```

3 - Move client to target machine along with `settings.json`
