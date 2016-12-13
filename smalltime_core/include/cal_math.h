#pragma once
#ifndef _CALMATH_
#define _CALMATH_

#include "core_decls.h"
#include "core_math.h"
#include "chrono_decls.h"

namespace smalltime
{
	namespace math
	{

		//========================================================================
		// Extract whole number portion of FixedDateTime double
		//========================================================================
		inline RD ExtractDate(RD dtm)
		{
			return std::floor(dtm);

		}

		//=================================================================
		// Find the day of week for week since R.D.0
		//================================================================
		inline int FixedDayOfWeek(RD rd)
		{
			double Sunday = 0.0;
			return static_cast<int>(FlMod((rd - KRD0 + Sunday), 7));
		}

		//=========================================================
		// Find number of weeks since R.D.0
		//==============================================================
		inline int FixedNumOfWeeks(RD rd)
		{
			double Sunday = 0.0;
			return static_cast<int>(std::floor(rd - KRD0 + Sunday) / 7);
		}


		RD GetFirstOfCycleDate(RD startDate, int minDaysInFirstWeek);
		// days and weeks between to fixed points
		int FixedWeeksBetween(RD rd0, RD rd1);
		int FixedDaysBetween(RD rd0, RD rd1);
		// day of week utility functions
		RD KDayOnOrBefore(int k, RD rdate);
		RD KDayOnOrAfter(int k, RD rdate);

		RD KDayAfter(int k, RD rdate);
		RD KDayBefore(int k, RD rdate);

		RD KDayNearest(int k, RD rdate);
		// Nth occurence of K day of week from rdate
		RD NthKDay(int k, int n, RD rdate);

	}
}

#endif
