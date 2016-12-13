#pragma once
#ifndef _MATHUTILS_
#define _MATHUTILS_

#include "core_decls.h"
#include <cctype>
#include <cmath>

namespace smalltime
{
	namespace math
	{
		static const RD KPI = 3.14159265358979323846;

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

		//==============================================
		//  ASTOR - Arc-seconds to radians
		//==============================================
		inline RD Astor(RD rd)
		{
			return rd * (KPI/ (180.0 * 3600.0));
		}

		//============================================
		//  DTR  --  Degrees to radians. 
		//============================================
		inline RD DegToRad(RD d)
		{
			return (d * KPI) / 180.0;
		}

		//========================================
		//  RTD  --  Radians to degrees.
		//======================================
		inline RD RadToDeg(RD r)
		{
			return (r * 180.0) / KPI;
		}

		//======================================================
		//  FIXANGLE  --  Range reduce angle in degrees.
		//=====================================================
		inline RD FixAngle(RD a)
		{
			return a - 360.0 * (std::floor(a / 360.0));
		}

		//=====================================================
		//  FIXANGR  --  Range reduce angle in radians.  
		//======================================================
		inline RD FixAngr(RD a)
		{
			return a - (2.0 * KPI) * (std::floor(a / (2.0 * KPI)));
		}

		//==============================================
		//  DSIN  --  Sine of an angle in degrees
		//=============================================
		inline RD DegSin(RD d)
		{
			return std::sin(DegToRad(d));
		}

		//=============================================
		//  DCOS  --  Cosine of an angle in degrees
		//===============================================
		inline RD DegCos(RD d)
		{
			return std::cos(DegToRad(d));
		}

	
		// hash string to create unique ID
		uint32_t GetUniqueID(const std::string& str);
		// utility functions to store and extract characters from integers
		uint32_t Pack4Chars(std::string str);
		uint64_t Pack8Chars(std::string str);
		std::string Unpack4Chars(uint32_t num);
		std::string Unpack8Chars(uint64_t num);
	}
}

#endif