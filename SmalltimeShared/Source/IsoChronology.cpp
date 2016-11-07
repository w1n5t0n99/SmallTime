#include "..\Include\IsoChronology.h"
#include "..\Include\ChronoDecls.h"
#include "..\Include\CalMath.h"
#include "..\Include\TimeMath.h"

namespace smalltime
{
	namespace chrono
	{
		static constexpr double Year4 = 1.0 / 4.0;
		static constexpr double Year100 = 1.0 / 100.0;
		static constexpr double Year400 = 1.0 / 400.0;

		//============================================================
		// Calculate the RD format from the year, month, day format
		//============================================================
		RD IsoChronology::rdFromYmd(int year, int month, int day) const
		{
			//If month out of range (e.g. 13) we'll catch that error as bad date
			//Want to throw a bad date exception not an array out of range exception
			int monthIndex = (month - 1) % 12;
			double m = iso_greg_julian::STD_MONTH_BIAS[monthIndex];
			//double y = year - Floor(m / 10.0);
			double y = static_cast<double>(year) - iso_greg_julian::STD_YEAR_BIAS[monthIndex];

			double leap4 = std::floor(y * Year4);
			double leap100 = std::floor(y * Year100);
			double leap400 = std::floor(y * Year400);

			double monDays = iso_greg_julian::STD_BASE_MONTH_DAYS[monthIndex];

			RD rd = ISO_EPOCH - 307.0 + (365.0 * y) + leap4 - leap100 + leap400 + monDays + 30.0 * m + day;

			return rd;
		}

		//====================================================================
		// Calculate the RD format from the year, week, day format
		//====================================================================
		RD IsoChronology::rdFromYwd(int year, int week, int day) const
		{
			//the amount of day that can fall in previous cycle, add 1 to bias for week starting on Mon and Sun being end of week
			//To find the earliest Mon we just move Sun back 1 day
			int maxDaysOfPrevYear = 7 - m_minDaysInFirstWeek + 1;
			return math::nthKDay(0, week, rdFromYmd(year, 1, 1) - maxDaysOfPrevYear) + day;
		}

		//==================================================================
		// Calculate the RD format from year, day format
		//==================================================================
		RD IsoChronology::rdFromYd(int year, int day) const
		{
			RD fp = rdFromYmd(year, 1, 1);
			return fp + static_cast<RD>(day) - 1;
		}

		//=====================================================
		// Calculate the YMD format from RD format
		//=====================================================
		YMD IsoChronology::ymdFromRd(RD rd) const
		{
			RD rdOnly = math::extractDate(rd);

			int y = determineYearFromFixed(ISO_EPOCH - 1.0 + rdOnly + 306.0);
			double priorDays = rdOnly - rdFromYmd(y - 1, 3, 1);

			double m = std::floor((1.0 / 153.0) * (5.0 * priorDays + 2));
			double month = math::aFlMod(m + 3.0, 12);

			double year = y - std::floor((month + 9.0) / 12.0);
			double day = rdOnly - rdFromYmd(year, month, 1) + 1.0;

			return{ static_cast<int>(year), static_cast<int>(month), static_cast<int>(day) };
		}

		//===================================================
		// Calculate the yd format from the RD format
		//===================================================
		YD IsoChronology::ydFromRd(RD rd) const
		{
			RD rdOnly = math::extractDate(rd);
			int year = determineYearFromFixed(rdOnly);   //<== remove for optimization

			RD firstOfYear = rdFromYmd(year, 1, 1);

			return{ year, static_cast<int>(std::floor(rdOnly) - std::floor(firstOfYear)) + 1 };
		}

		//==================================================
		// Calculate YWD format from RD format
		//==================================================
		YWD IsoChronology::ywdFromRd(RD rd) const
		{
			int year = determineYearFromFixed(rd);   //<== remove for optimization
			RD rdOnly = math::extractDate(rd);

			int nextYear = year + 1;
			int prevYear = year - 1;
			YWD ywd;

			RD firstOfYear = rdFromYwd(nextYear, 1, 1);
			if (rdOnly >= firstOfYear)
			{
				double dWeek = 1.0 + std::floor((1.0 / 7.0) * (rdOnly - firstOfYear));
				ywd[1] = static_cast<int>(dWeek);
				ywd[0] = nextYear;
			}
			else if (rdOnly >= (firstOfYear = rdFromYwd( year, 1, 1)))
			{
				double dWeek = 1.0 + std::floor((1.0 / 7.0) * (rdOnly - firstOfYear));
				ywd[1] = static_cast<int>(dWeek);
				ywd[0] = year;
			}
			else if (rdOnly >= (firstOfYear = rdFromYwd(prevYear, 1, 1)))
			{
				double dWeek = 1.0 + std::floor((1.0 / 7.0) * (rdOnly - firstOfYear));
				ywd[1] = static_cast<int>(dWeek);
				ywd[0] = prevYear;
			}

			//double dDay = FixedDayOfWeek(rdOnly - RD0);
			double dDay = math::aFlMod(rdOnly - RD0, 7);
			ywd[2] = static_cast<int>(dDay);
			return ywd;
		}

		//==========================================================
		// Find the year a given FixedDateTime falls in
		//==========================================================
		int IsoChronology::determineYearFromFixed(RD rd) const
		{
			double d0 = math::extractDate(rd) - ISO_EPOCH;

			double d0Frac = d0 / 146097;
			double n400 = std::floor(d0Frac);
			double d1 = math::quickFlMod(d0, 146097, d0Frac);

			double d1Frac = d1 / 36524;
			double n100 = std::floor(d1Frac);
			double d2 = math::quickFlMod(d1, 36524, d1Frac);

			double d2Frac = d2 / 1461;
			double n4 = std::floor(d2Frac);
			double d3 = math::quickFlMod(d2, 1461, d2Frac);

			double n1 = std::floor(d3 / 365);

			double year = (400 * n400) + (100 * n100) + (4 * n4) + n1;

			if (n100 == 4 || n1 == 4)
				year += 0;
			else
				year += 1;

			return static_cast<int>(year);
		}

		//============================================================
		// Calculate the date relative to another date
		//=============================================================
		RD IsoChronology::rdRelativeTo(RD rd, RelSpec rel) const
		{
			RD relativeRd = 0.0;
			// Find the date relative to the specifier ==========================
			int dow = static_cast<int>(rel) % 7;

			int relSpecIndex = static_cast<int>(rel);
			//OnOrAfter
			if (relSpecIndex >= 7 && relSpecIndex < 14)
			{
				relativeRd = math::kdayOnOrAfter(dow, rd);
			}
			//After
			else if (relSpecIndex >= 14 && relSpecIndex < 21)
			{
				relativeRd = math::kdayAfter(dow, rd);
			}
			//OnOrBefore
			else if (relSpecIndex >= 21 && relSpecIndex < 28)
			{
				relativeRd = math::kdayOnOrBefore(dow, rd);
			}
			//Before
			else if (relSpecIndex >= 28 && relSpecIndex < 35)
			{
				relativeRd = math::kdayBefore(dow, rd);
			}
			//Last
			else if (relSpecIndex >= 35 && relSpecIndex < 42)
			{
				auto ymd = ymdFromRd(rd);
				// increment month
				ymd[1] += 1;
				ymd[2] = 1;
				// increment year if past last month
				if (ymd[1] == 13)
				{
					ymd[0] += 1;
					ymd[1] = 1;
				}

				auto newRD = rdFromYmd(ymd[0], ymd[1], ymd[2]);
				relativeRd = math::kdayBefore(dow, newRD);
			}

			return relativeRd;
		}

		//=====================================================
		// Calculate RD format from time fields
		//=====================================================
		RD IsoChronology::rdFromTime(int hour, int min, int sec, int milli) const
		{
			return math::rdFromTime(hour, min, sec, milli);
		}

		//====================================================
		// calculate time fields from RD format
		//====================================================
		HMS IsoChronology::timeFromRd(RD rd) const
		{
			return math::hmsFromRd(rd);
		}

		//=============================================================================
		// calculate the week of month with min amount of days in first week
		//=============================================================================
		int IsoChronology::weekOfMonth(const YWD& ywd, RD rd) const
		{
			int isoWeekBias = 1;
			int week;
			//Add 1 to bias for Monday as start of week
			RD firstOfMonth = math::getFirstOfCycleDate(rdFromYmd(ywd[0], ywd[1], 1), m_minDaysInFirstWeek- 1) + isoWeekBias;
			if (rd < firstOfMonth)
				week = 0;
			else
			{
				RD diff = rd - firstOfMonth;
				week = static_cast<int>(1.0 + std::floor((1.0 / 7.0) * diff));
			}

			return week;

		}

		//=====================================================
		// Find if the given year is a leap year
		//=====================================================
		bool IsoChronology::isLeapYear(int year) const
		{
			unsigned int absYear = abs(year);
			const unsigned int leap100 = 100;
			const unsigned int leap400 = 400;

			if (math::fastMod4(absYear) == 0)
			{
				if (absYear % leap100 == 0)
				{
					if (absYear % leap400 == 0)
						return true;
					else
						return false;
				}
				return true;
			}
			return false;
		}

	}
}