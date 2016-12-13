#include "../include/julian_chronology.h"
#include "../include/chrono_decls.h"
#include "../include/cal_math.h"
#include "../include/time_math.h"

namespace smalltime
{
	namespace chrono
	{
		static const RD KJULIAN_EPOCH = -1.0;

		//============================================================
		// Calculate the RD format from the year, month, day format
		//============================================================
		RD JulianChronology::FixedFromYmd(int year, int month, int day) const
		{
			RD d_year = static_cast<RD>(year);
			RD d_month = static_cast<RD>(month);
			RD d_day = static_cast<RD>(day);

			RD y = 0.0;
			if (d_year < 0.0)
				y = d_year + 1.0;
			else
				y = d_year;

			RD lp = 0.0;
			if (month <= 2)
				lp = 0.0;
			else if (IsLeapYear(year))
				lp = -1.0;
			else
				lp = -2.0;

			return KJULIAN_EPOCH - 1.0 + 365.0 * (y - 1.0) + std::floor((y - 1.0) / 4.0) +
				std::floor((1.0 / 12.0) * (367.0 * d_month - 362.0)) + lp + d_day;
		}

		//=====================================================
		// Calculate the YMD format from RD format
		//=====================================================
		YMD JulianChronology::YmdFromFixed(RD rd) const
		{
			RD dt_only = math::ExtractDate(rd);

			RD d_approx = std::floor((1.0 / 1461.0) * (4.0 * (dt_only - KJULIAN_EPOCH) + 1464.0));
			int  approx = static_cast<int>(d_approx);

			int year = 0, month = 0, day = 0;
			if (d_approx <= 0.0)
				year = approx - 1;
			else
				year = approx;

			double prior_days = dt_only - FixedFromYmd(year, 1, 1);
			double correction = 0.0;

			if (dt_only < FixedFromYmd(year, 3, 1))
				correction = 0.0;
			else if (IsLeapYear(year))
				correction = 1.0;
			else
				correction = 2.0;

			double d_month = std::floor((1.0 / 367.0) * (12.0 * (prior_days + correction) + 373.0));
			month = static_cast<int>(d_month);

			double d_day = dt_only - FixedFromYmd(year, month, 1) + 1.0;
			day = static_cast<int>(d_day);

			return{ year, month, day };
		}

		//==========================================================
		// Find the year a given FixedDateTime falls in
		//==========================================================
		int JulianChronology::DetermineYearFromFixed(RD rd) const
		{
			double d0 = rd - KJULIAN_EPOCH; 

			double d0Frac = d0 / 146097;
			double n400 = std::floor(d0Frac);
			double d1 = math::QuickFlMod(d0, 146097, d0Frac);

			double d1Frac = d1 / 36524;
			double n100 = std::floor(d1Frac);
			double d2 = math::QuickFlMod(d1, 36524, d1Frac);

			double d2Frac = d2 / 1461;
			double n4 = std::floor(d2Frac);
			double d3 = math::QuickFlMod(d2, 1461, d2Frac);

			double n1 = std::floor(d3 / 365);

			double year = (400 * n400) + (100 * n100) + (4 * n4) + n1;

			if (n100 == 4 || n1 == 4)
				year += 0;
			else
				year += 1;

			return (int)year;
		}

		//============================================================
		// Calculate the date relative to another date
		//=============================================================
		RD JulianChronology::FixedRelativeTo(RD rd, RS rel) const
		{
			RD relative_rd = 0.0, incr_rd = 0.0, good_rd = 0.0;
			YMD ymd = { 0,0,0 }, incr_ymd = { 0, 0, 0 };
			// Find the date relative to the specifier ==========================
			int dow = static_cast<int>(rel) % 7;
			int rel_spec_index = static_cast<int>(rel);

			switch (rel_spec_index)
			{
				//OnOrAfter
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				relative_rd = math::KDayOnOrAfter(dow, rd);
				break;
				//After
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
				relative_rd = math::KDayAfter(dow, rd);
				//OnOrBefore
				break;
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
				relative_rd = math::KDayOnOrBefore(dow, rd);
				//Before
				break;
			case 28:
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
				relative_rd = math::KDayBefore(dow, rd);
				break;
				//Last
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
				ymd = YmdFromFixed(rd);
				// increment month
				ymd[1] += 1;
				ymd[2] = 1;

				//check if valid date and increment year if not
				incr_rd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				incr_ymd = YmdFromFixed(incr_rd);

				if (ymd[0] != incr_ymd[0] || ymd[1] != incr_ymd[1] || ymd[2] != incr_ymd[2])
				{
					ymd[0] += 1;
					ymd[1] = 1;
				}

				good_rd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				relative_rd = math::KDayBefore(dow, good_rd);
				break;
			case 42:
				ymd = YmdFromFixed(rd);
				// increment month
				ymd[1] += 1;
				ymd[2] = 1;

				//check if valid date and increment year if not
				incr_rd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				incr_ymd = YmdFromFixed(incr_rd);

				if (ymd[0] != incr_ymd[0] || ymd[1] != incr_ymd[1] || ymd[2] != incr_ymd[2])
				{
					ymd[0] += 1;
					ymd[1] = 1;
				}

				auto good_rd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				relative_rd = good_rd - 1.0;
				break;
			}

			return relative_rd;
		}

		//=====================================================
		// Calculate RD format from time fields
		//=====================================================
		RD JulianChronology::FixedFromTime(int hour, int min, int sec, int milli) const
		{
			return math::FixedFromTime(hour, min, sec, milli);
		}

		//====================================================
		// calculate time fields from RD format
		//====================================================
		HMS JulianChronology::TimeFromFixed(RD rd) const
		{
			return math::HmsFromFixed(rd);
		}

		//=====================================================
		// Find if the given year is a leap year
		//=====================================================
		bool JulianChronology::IsLeapYear(int year) const
		{
			if (year > 0)
			{
				if (math::FlMod(year, 4.0) == 0.0)
					return true;
			}
			else
			{
				if (math::FlMod(year, 4.0) == 3.0)
					return true;
			}

			return false;
		}

	}
}