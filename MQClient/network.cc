/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/


#include <windows.h>
#include <wininet.h>
#include <ws2tcpip.h>
#include <cassert>
#include <iostream>
#include <iterator>
#include "network.hpp"
#include "tcp_client.hpp"
#include "log.hpp"

using namespace std;

// Inicializa a winsock library
bool Network::Utils::LoadWinsock()
{
	WSADATA data;

	return (WSAStartup(MAKEWORD(2,2), &data) == 0);
}

// Finaliza a winsock library
bool Network::Utils::FreeWinsock()
{
	return(WSACleanup() != SOCKET_ERROR);
}

// Realiza o download de um arquivo da URL especificada
bool Network::Utils::DownloadFile(string url, string local_filename)
{
	HRESULT hRes;
	
	assert(!(url.empty() || local_filename.empty()));
	
	// Download file
	hRes = URLDownloadToFileA(0, url.c_str(), local_filename.c_str(), 0, NULL);
	
	return hRes == S_OK;
}

bool Network::Utils::IsInternetConnected()
{
	DWORD state = 0;
	
	if(InternetGetConnectedState(&state, 0))
		if(state & INTERNET_CONNECTION_LAN)
			return true;

	return false;
}

/*
	Obtém o endereço IPV4 Externo da máquina
*/
string Network::Utils::GetExternalIPV4Addr()
{
	string host = "api.ipify.org";
	string format = "/?format=html";
	const string header_end = "\r\n\r\n";
	string request = "GET " + format + header_end;
	const int buflen = 512;
	char *response = new char[buflen];
	string host_addr = Network::Utils::SolveIPAddress(host);

	TcpClient *client = new TcpClient(host_addr, 80);
	if(client->Connect()) {
		client->Send(request.c_str(), request.length());
		client->Recv(&response, buflen);
	} else {
		Log::LogError("[ERROR] Failed to connect with " + host_addr);
		return "";
	}

	string s_response = string(response);
	size_t header_index = s_response.find(header_end);
	string body = s_response.substr(header_index + header_end.length());

	return body;
}

string Network::Utils::SolveIPAddress(string hostname)
{
	struct addrinfo hints, *result = NULL;
	struct addrinfo *p = NULL;

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if(getaddrinfo(hostname.c_str(), "0", &hints, &result)) {
		Log::LogError("[ERROR] Failed: " + string(strerror(errno)) + ", error: " + to_string(WSAGetLastError()));
		return "";
	}

	for(p = result; p != NULL; p = p->ai_next) {
		if(p->ai_family == AF_INET) {
			return inet_ntoa(((struct sockaddr_in*)p->ai_addr)->sin_addr);
		}
	}

    return "";
}
