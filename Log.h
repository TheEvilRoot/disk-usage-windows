#pragma once

#include <string>
#include <iostream>
#include <iomanip>

namespace Log {

	inline std::ostream& fileSize(double value, int precision = 1, int reserved = 12)
	{
		std::cout 
			<< std::fixed 
			<< std::setprecision(precision) 
			<< std::setfill(' ') 
			<< std::setw(reserved)
			<< value;
		return std::cout;
	}

	inline std::ostream& warning()
	{
		std::cout << "[Warining] ";
		return std::cout;
	}

	inline std::ostream& error()
	{
		std::cerr << "[Error] ";
		return std::cerr;
	}

	inline std::ostream& info()
	{
		std::cout << "[Info] ";
		return std::cout;
	}
}