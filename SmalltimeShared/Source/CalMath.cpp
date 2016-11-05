#include "../Include/CalMath.h"

#include <cmath>

namespace smalltime
{
	namespace math
	{
		//=================================================================
		// Return the start of cycle, e.g. month or year with the min
		// amount of days in week. Start of week is Sunday
		//================================================================
		RD getFirstOfCycleDate(RD startDate, int minDaysInFirstWeek)
		{
			//the amount of day that can fall in previous cycle
			int maxDaysOfPrevYear = 7 - minDaysInFirstWeek;
			RD earliestFirstWeekDate = startDate - maxDaysOfPrevYear;
			//find next sunday closest to earliestFirstWeekDate
			RD wd = nthKDay(0, 1, earliestFirstWeekDate);

			return wd;

		}

		//==================================================================
		// Find number of weeks between two dates
		//==================================================================
		int fixedWeeksBetween(RD rd0, RD rd1)
		{
			//find week of the month
			int week0 = fixedNumOfWeeks(rd0);
			int week1 = fixedNumOfWeeks(rd1);

			return std::abs(week0 - week1);

		}

		//==================================================================
		// Find number of days between two dates
		//==================================================================
		int fixedDaysBetween(RD rd0, RD rd1)
		{
			return static_cast<int>(std::abs(rd0 - rd1));

		}

		//==========================================================================
		// find occurrence of the Nth day of a weekly cycle on or before rdate
		//==========================================================================
		double kdayOnOrBefore(int k, RD rdate)
		{
			//the R.D epoch is Mon 1 so 0 is on a Sun
			int dayZero = 0;
			int mCycle = 7;
			double day = 0;

			day = rdate - flMod((rdate + dayZero - k), mCycle);
			//	day = std::floor(day);
			return day;

		}

		//=========================================================================
		// find occurrence of the Nth day of a weekly cycle on or before rdate
		//==========================================================================
		double kdayAfter(int k, double rdate)
		{
			return kdayOnOrBefore(k, rdate + 7.0);

		}

		//==========================================================================
		// find occurrence of the Nth day of a weekly cycle before rdate
		//==========================================================================
		double kdayBefore(int k, double rdate)
		{
			return kdayOnOrBefore(k, rdate - 1.0);

		}

		//==========================================================================
		// find occurrence of the Nth day of a weekly cycle before rdate
		//==========================================================================
		double kdayNearest(int k, double rdate)
		{
			return kdayOnOrBefore(k, rdate + 3.0);

		}

		//==========================================================================
		// find occurrence of the Nth day of a weekly cycle before rdate
		//==========================================================================
		double kdayOnOrAfter(int k, double rdate)
		{
			return kdayAfter(k, rdate - 1.0);

		}

		//===================================================================================
		// find Nth occurence of the kday from rdate, N is pos for after and neg for before
		//===================================================================================
		double nthKDay(int k, int n, double rdate)
		{
			double kdate;
			//if first occurence falls on k it is counted
			if (n > 0)
			{
				kdate = kdayBefore(k, rdate);
				kdate += (7 * n);

				return kdate;
			}
			else if (n < 0)
			{
				kdate = kdayAfter(k, rdate);
				kdate += (7 * n);

				return kdate;

			}
			else
				return rdate;

		}
	}
}