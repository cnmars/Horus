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

using namespace std;

namespace API {
    class FileSystem {
        public:
            static vector<string> ListFiles();
    };
}

#endif

