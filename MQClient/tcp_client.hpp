/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <memory>
#include "network.hpp"

using namespace std;

/**
 * @brief Handle to TCP client
 * 
 */
class TcpClient {
    public:
        /**
         * @brief Construct a new Tcp Client object
         * 
         */
        TcpClient();

        /**
         * @brief Construct a new Tcp Client object
         * 
         * @param ip_address 
         * @param port 
         */
        explicit TcpClient(string ip_address, uint16_t port);

        /**
         * @brief Destroy the Tcp Client object
         * 
         */
        ~TcpClient();

        /**
         * @brief Connects the client to specified IP
         * 
         * @return true 
         * @return false 
         */
        bool Connect();

        /**
         * @brief Sends data to remote endpoint
         * 
         * @param data 
         * @param len 
         * @return int 
         */
        int Send(const void *data, size_t len);

        /**
         * @brief Receives data from remote endpoint
         * 
         * @param buf 
         * @return int 
         */
        int Recv(char **buf, size_t len);

        /**
         * @brief Cleanup client
         * 
         */
        void Cleanup();
    private:
            /**
         * @brief Initializes the TCP client
         * 
         * @return true 
         * @return false 
         */
        bool Initialize();

    private:
        SOCKET sock;
        string ip_addr;
        uint16_t port;
};
