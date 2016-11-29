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
		inline double FlMod(double x, double y) 
		{
			return x - y * std::floor(x / y);

		}

		//=========================================================================
		// quick mod function when the qoutient has been pre-computed
		//========================================================================
		inline double QuickFlMod(double x, double y, double qXY)
		{
			return x - y * std::floor(qXY);

		}

		//========================================================================
		// adjusted mod method that uses floor instead of truncation
		//==========================================================================
		inline double AFlMod(double x, double y)
		{
			//return x - y * ((x / y) - 1);
			return (FlMod(x, y) == 0) ? y : FlMod(x, y);

		}

		//=========================================================================
		// Fast bitwise mod by 4, Compilers prob do implicitly with consts but
		// just in case
		//=========================================================================
		inline constexpr unsigned int FastMod4(unsigned int n)
		{
			// n& (d-1) where d = 4
			return n& (3);
		}

	
		// hash string to create unique ID
		uint32_t GetUniqueID(const std::string& str);
		// Return the complement of the fractional part of floating point
		//RD getComplementOfFractional(RD num);

		// utility functions to store and extract characters from integers
		uint32_t Pack4Chars(std::string str);
		uint64_t Pack8Chars(std::string str);
		std::string Unpack4Chars(uint32_t num);
		std::string Unpack8Chars(uint64_t num);
	}
}

#endif