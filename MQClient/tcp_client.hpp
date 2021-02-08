/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <memory>

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
        explicit TcpClient(std::string ip_address, uint16_t port);

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

        /**
         * @brief 
         * 
         * @param addr_info 
         * @return true 
         * @return false 
         */
        bool Connect(void *addr_info);

private:
            /**
         * @brief Initializes the TCP client
         * 
         * @return true 
         * @return false 
         */
        bool Initialize();

        /**
         * @brief 
         * 
         * @param addr_info 
         * @return true 
         * @return false 
         */
        bool Initialize(void *addr_info);

    private:
        SOCKET sock;
        std::string ip_addr;
        uint16_t port;
};

#endif
