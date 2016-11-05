#pragma once
#ifndef _MATHUTILS_
#define _MATHUTILS_

#include "CoreDecls.h"
#include <cctype>
#include <cmath>

namespace smalltime
{
	namespace math
	{

		//==========================================================================
		// modulus method that uses floor instead of trucation
		//==============================================================================
		inline double flMod(double x, double y) 
		{
			return x - y * std::floor(x / y);

		}

		//=========================================================================
		// quick mod function when the qoutient has been pre-computed
		//========================================================================
		inline double quickFlMod(double x, double y, double qXY)
		{
			return x - y * std::floor(qXY);

		}

		//========================================================================
		// adjusted mod method that uses floor instead of truncation
		//==========================================================================
		inline double aFlMod(double x, double y)
		{
			//return x - y * ((x / y) - 1);
			return (flMod(x, y) == 0) ? y : flMod(x, y);

		}

		//=========================================================================
		// Fast bitwise mod by 4, Compilers prob do implicitly with consts but
		// just in case
		//=========================================================================
		inline constexpr unsigned int fastMod4(unsigned int n)
		{
			// n& (d-1) where d = 4
			return n& (3);
		}

	
		// hash string to create unique ID
		uint32_t getUniqueID(const std::string& str);
		// Return the complement of the fractional part of floating point
		//RD getComplementOfFractional(RD num);

		// utility functions to store and extract characters from integers
		uint32_t pack4Chars(std::string str);
		uint64_t pack8Chars(std::string str);
		std::string unpack4Chars(uint32_t num);
		std::string unpack8Chars(uint64_t num);
	}
}

#endif