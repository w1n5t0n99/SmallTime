#pragma once
#ifndef _CALMATH_
#define _CALMATH_

#include "CoreDecls.h"
#include "MathUtils.h"

namespace smalltime
{
	namespace math
	{
		//========================================================================
		// Extract whole number portion of FixedDateTime double
		//========================================================================
		inline RD extractDate(RD dtm)
		{
			return std::floor(dtm);

		}

		//=================================================================
		// Find the day of week for week since R.D.0
		//================================================================
		inline int fixedDayOfWeek(RD rd)
		{
			double Sunday = 0.0;
			return static_cast<int>(flMod((rd - RD0 + Sunday), 7));
		}

		//=========================================================
		// Find number of weeks since R.D.0
		//==============================================================
		inline int fixedNumOfWeeks(RD rd)
		{
			double Sunday = 0.0;
			return static_cast<int>(std::floor(rd - RD0 + Sunday) / 7);
		}


		RD getFirstOfCycleDate(RD startDate, int minDaysInFirstWeek);
		// days and weeks between to fixed points
		int fixedWeeksBetween(RD rd0, RD rd1);
		int fixedDaysBetween(RD rd0, RD rd1);
		// day of week utility functions
		RD kdayOnOrBefore(int k, RD rdate);
		RD kdayOnOrAfter(int k, RD rdate);

		RD kdayAfter(int k, RD rdate);
		RD kdayBefore(int k, RD rdate);

		RD kdayNearest(int k, RD rdate);
		// Nth occurence of K day of week from rdate
		RD nthKDay(int k, int n, RD rdate);

	}
}

#endif
