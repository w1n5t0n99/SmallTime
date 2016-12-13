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

		//=======================================
		// Calculate the local mean time zone
		//========================================
		inline RD ZoneFromLongitude(RD longitude)
		{
			return longitude / 360.0;
		}

		//====================================================
		// Calculate universal time from local mean time
		//====================================================
		inline RD UniversalFromLocal(RD local_time, Locale locale)
		{
			return local_time - ZoneFromLongitude(locale.longitude);
		}

		//====================================================
		// Calculate local mean time from universal time
		//====================================================
		inline RD LocalFromUniversal(RD unv_time, Locale locale)
		{
			return unv_time + ZoneFromLongitude(locale.longitude);
		}

		//====================================================
		// Calculate standard time from universal
		//====================================================
		inline RD StandardFromUniversal(RD unv_time, Locale locale)
		{
			return unv_time + locale.zone;
		}

		//====================================================
		// Calculate universal from standard
		//====================================================
		inline RD UniversalFromStandard(RD std_time, Locale locale)
		{
			return std_time - locale.zone;
		}

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
