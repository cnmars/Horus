/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

using namespace std;

/**
 * @brief General utilities
 * 
 */
class Utils {
public:
	/**
	 * @brief Generates a random seed
	 * 
	 */
	static void GenerateRandomSeed();

	/**
	 * @brief Get a random positive integer
	 * 
	 * @return int 
	 */
	static int GetRandomPositive();

	/**
	 * @brief 
	 * 
	 * @return string an string containing a ID
	 */
	static string GenerateID();
	
	/**
	 * @brief Transforms a string to lowercase
	 * 
	 * @param s String
	 * @return string Lowercase string 
	 */
	static string ToLower(string& s);

	/**
	 * @brief Splits the specified string s using the delimiter delim
	 * 
	 * @param s String to split
	 * @param t Splitted string
	 * @param delim Delimiter
	 */
	static vector<string> Split(string s, char delim);
};

#endif
