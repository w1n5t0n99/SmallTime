#pragma once
#ifndef _TIMEMATH_
#define _TIMEMATH_

#include "core_decls.h"

namespace smalltime
{
	namespace math
	{
		// Time globals
		//Milliseconds in standard day, no leap second applied
		static constexpr RD KMILLISECONDS_IN_DAY = 86400000.0;
		static constexpr RD KMILLISECONDS_IN__HOUR = KMILLISECONDS_IN_DAY / 24.0;
		static constexpr RD KMILLISECONDS_IN_MINUTE = KMILLISECONDS_IN__HOUR / 60.0;
		static constexpr RD KMILLISECONDS_IN_SECOND = KMILLISECONDS_IN_MINUTE / 60.0;

		//========================================================================
		// Extract fractional portion of FixedDateTime double
		//========================================================================
		inline RD ExtractTime(const RD& dtm)
		{
			return dtm - std::floor(dtm);
		}

		//===========================================
		// convert time fields to fixed format
		//===========================================
		constexpr RD FixedFromTime(int h, int m, int s, int ms)
		{
			// HoursInMillis + MinsInMillis + SecsInMillis + Millis / MillisInDay
			return ((h * 60.0 * 60.0 * 1000.0) + (m * 60 * 1000.0) + (s * 1000.0) + ms) / KMILLISECONDS_IN_DAY;
		}

		//============================================
		// FixedPoint representation of 1 ms
		//============================================
		constexpr RD MSEC()
		{
			return FixedFromTime(0, 0, 0, 1);
		}

		//============================================
		// FixedPoint representation of 1 second
		//============================================
		constexpr RD SEC()
		{
			return FixedFromTime(0, 0, 1, 0);
		}

		//============================================
		// FixedPoint representation of 1 min
		//===========================================
		constexpr RD MIN()
		{
			return FixedFromTime(0, 1, 0, 0);
		}

		//============================================
		// FixedPoint representation of 1 hour
		//===========================================
		constexpr RD HOUR()
		{
			return FixedFromTime(1, 0, 0, 0);
		}

		HMS HmsFromFixed(RD rd);
	}
}

#endif
