/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>
#include <memory>

using namespace std;

/**
 * @brief Size of a IPV4 address
 * 
 */
#define IPV4_ADDR_LEN	16

#ifndef arraySize
/**
 * @brief Returns the number of elements in x
 * 
 */
#define arraySize(x)	(sizeof(x)/sizeof(x[0]))
#endif

#ifndef IS_NULL
/**
 * @brief Check if x is a null pointer
 * 
 */
#define IS_NULL(x) (x == nullptr)
#endif

struct ipv4_addr_info {
	void *sock_addr;
	string ip;
	size_t sock_addr_len;
	int ai_family;
	int ai_protocol;
	int ai_socktype;
};

/**
 * @brief Default network namespace
 * 
 */
namespace Network {
	class Utils {
		public:
			/**
			 * @brief Downloads a remote file into local computer
			 * 
			 * @param url URL to remote file
			 * @param local_filename Name of local file
			 * @return true On success
			 * @return false On error
			 */
			static bool DownloadFile(string url, string local_filename);

			/**
			 * @brief Checks if the program is connected to internet
			 * 
			 * @return true 
			 * @return false 
			 */
			static bool IsInternetConnected();

			/**
			 * @brief Load winsock library
			 * 
			 * @return true 
			 * @return false 
			 */
			static bool LoadWinsock();

			/**
			 * @brief Unload winsock library
			 * 
			 * @return true 
			 * @return false 
			 */
			static bool FreeWinsock();

			/**
			 * @brief Gets the external IP address for current host
			 * 
			 * @return string IPV4 address in string format
			 */
			static string GetExternalIPV4Addr();

			/**
			 * @brief Solve the IP address of provied host
			 * 
			 * @param hostname Name of host
			 * @return string Contains the ip address information
			 */
			static struct ipv4_addr_info *SolveIPAddress(string hostname);
	};
}

#endif