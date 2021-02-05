/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include "tcp_client.hpp"
#include "log.hpp"

TcpClient::TcpClient()
{
	Network::Utils::LoadWinsock();
    this->port = 0;
    this->ip_addr = "";
    this->sock = INVALID_SOCKET;
}

TcpClient::TcpClient(string ip, uint16_t port)
{
    Network::Utils::LoadWinsock();
    this->port = port;
    this->ip_addr = ip;
    this->sock = INVALID_SOCKET;
}

TcpClient::~TcpClient()
{
    Network::Utils::FreeWinsock();
}

bool TcpClient::Initialize()
{
    if(this->sock == INVALID_SOCKET || sock == SOCKET_ERROR)
        this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   
    return this->sock != INVALID_SOCKET;
}

bool TcpClient::Initialize(void *addr_info)
{
    ipv4_addr_info *info = static_cast<ipv4_addr_info*>(addr_info);
    if(this->sock == INVALID_SOCKET || sock == SOCKET_ERROR)
        this->sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    return this->sock != INVALID_SOCKET;
}

bool TcpClient::Connect()
{
    sockaddr_in sin;

    this->Initialize();

    sin.sin_family = AF_INET;
    sin.sin_port = htons(this->port);
    sin.sin_addr.S_un.S_addr = inet_addr(this->ip_addr.c_str());

    Log::LogInfo("Connecting to " + this->ip_addr + ":" + to_string(this->port));

    auto status = connect(this->sock, (const sockaddr*)&sin, sizeof(struct sockaddr_in));

    return status != SOCKET_ERROR;
}

bool TcpClient::Connect(void *addr_info)
{
    ipv4_addr_info *info = static_cast<ipv4_addr_info*>(addr_info);
    if(info != nullptr) {
        Log::LogInfo("Connecting to: " + info->ip);

        this->Initialize(addr_info);
    
        // Tries to connect to the server
        auto status = connect(this->sock, info->sock_addr, info->sock_addr_len);
        if(status != SOCKET_ERROR)
            return true;
    }

    return false;
}

int TcpClient::Send(const void *data, size_t len)
{
    char *pdata = new char[len];
    size_t bytes_sent = 0;
    int chunk_size = 512;

    do {
        bytes_sent += send(this->sock, const_cast<const char*>(&pdata[bytes_sent]), chunk_size, 0);
    } while(bytes_sent < len);

    // Free memory
    delete[] pdata;

    return static_cast<int>(bytes_sent);
}

void TcpClient::Cleanup()
{
    // Ensures that all data is sent/received
    shutdown(this->sock, 2);

    // Releases all resources used by socket
    closesocket(this->sock);
}

int TcpClient::Recv(char **buf, size_t len)
{ 
    int bytes_received = 0;
    char *data = new char[len];
    int chunk_size = 64;

    do {
        auto received = recv(this->sock, &data[bytes_received], chunk_size, 0);
        if(received == SOCKET_ERROR) {
            if(WSAGetLastError() == WSAEWOULDBLOCK)
                // Wait until data is available
                select(this->sock, nullptr, nullptr, nullptr, nullptr);
            else
                break;
        } else {
            bytes_received += received;
        }
    } while(bytes_received < len);

    // Copy data into buffer
    memcpy(*buf, data, bytes_received);

    // Free allocated memory
    delete[] data;

    // Returns the number of bytes received
    return bytes_received;
}
