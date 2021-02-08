/*
	This file is part of MQRat.
	
	Copyright (C) 2019  MQRat Project

	O autor não se responsabiliza por variantes (ou sources originais) deste software 
	que estejam sendo utilizadas para prejudicar terceiros.
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

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
	 * @return std::string an string containing a ID
	 */
	static std::string GenerateID();
	
	/**
	 * @brief Transforms a string to lowercase
	 * 
	 * @param s String
	 * @return std::string Lowercase string 
	 */
	static std::string ToLower(std::string& s);
};

#endif
