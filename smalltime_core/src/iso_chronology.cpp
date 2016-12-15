#include "../include/iso_chronology.h"
#include "../include/chrono_decls.h"
#include "../include/cal_math.h"
#include "../include/time_math.h"

namespace smalltime
{
	namespace chrono
	{
		// Diffrent calendar field representations
		using YMD = std::array<int, 3>;
		using YWD = std::array<int, 3>;
		using YD = std::array<int, 2>;
		using HMS = std::array<int, 4>;

		static const RD KISO_EPOCH = 1.0;
		static constexpr double KYear4 = 1.0 / 4.0;
		static constexpr double KYear100 = 1.0 / 100.0;
		static constexpr double KYear400 = 1.0 / 400.0;

		//============================================================
		// Calculate the RD format from the year, month, day format
		//============================================================
		RD IsoChronology::FixedFromYmd(int year, int month, int day) const
		{
			//If month out of range (e.g. 13) we'll catch that error as bad date
			//Want to throw a bad date exception not an array out of range exception
			int month_index = (month - 1) % 12;
			double m = iso_julian::KSTD_MONTH_BIAS[month_index];
			//double y = year - Floor(m / 10.0);
			double y = static_cast<double>(year) - iso_julian::KSTD_YEAR_BIAS[month_index];

			double leap4 = std::floor(y * KYear4);
			double leap100 = std::floor(y * KYear100);
			double leap400 = std::floor(y * KYear400);

			double month_days = iso_julian::KSTD_BASE_MONTH_DAYS[month_index];

			RD rd = KISO_EPOCH - 307.0 + (365.0 * y) + leap4 - leap100 + leap400 + month_days + 30.0 * m + day;

			return rd;
		}

		//====================================================================
		// Calculate the RD format from the year, week, day format
		//====================================================================
		RD IsoChronology::FixedFromYwd(int year, int week, int day) const
		{
			//the amount of day that can fall in previous cycle, add 1 to bias for week starting on Mon and Sun being end of week
			//To find the earliest Mon we just move Sun back 1 day
			int max_days_of_prev_year = 7 - min_days_in_first_week_ + 1;
			return math::NthKDay(0, week, FixedFromYmd(year, 1, 1) - max_days_of_prev_year) + day;
		}

		//==================================================================
		// Calculate the RD format from year, day format
		//==================================================================
		RD IsoChronology::FixedFromYd(int year, int day) const
		{
			RD fp = FixedFromYmd(year, 1, 1);
			return fp + static_cast<RD>(day) - 1;
		}

		//=====================================================
		// Calculate the YMD format from RD format
		//=====================================================
		YMD IsoChronology::YmdFromFixed(RD rd) const
		{
			RD rd_only = math::ExtractDate(rd);

			int y = DetermineYearFromFixed(KISO_EPOCH - 1.0 + rd_only + 306.0);
			double prior_days = rd_only - FixedFromYmd(y - 1, 3, 1);

			double m = std::floor((1.0 / 153.0) * (5.0 * prior_days + 2));
			double month = math::AFlMod(m + 3.0, 12.0);

			double year = y - std::floor((month + 9.0) / 12.0);
			double day = rd_only - FixedFromYmd(year, month, 1) + 1.0;

			return{ static_cast<int>(year), static_cast<int>(month), static_cast<int>(day) };
		}

		//===================================================
		// Calculate the yd format from the RD format
		//===================================================
		YD IsoChronology::YdFromFixed(RD rd) const
		{
			RD rd_only = math::ExtractDate(rd);
			int year = DetermineYearFromFixed(rd_only);   //<== remove for optimization

			RD first_of_year = FixedFromYmd(year, 1, 1);

			return{ year, static_cast<int>(std::floor(rd_only) - std::floor(first_of_year)) + 1 };
		}

		//==================================================
		// Calculate YWD format from RD format
		//==================================================
		YWD IsoChronology::YwdFromFixed(RD rd) const
		{
			int year = DetermineYearFromFixed(rd);   //<== remove for optimization
			RD rd_only = math::ExtractDate(rd);

			int next_year = year + 1;
			int prev_year = year - 1;
			YWD ywd;

			RD firstOfYear = FixedFromYwd(next_year, 1, 1);
			if (rd_only >= firstOfYear)
			{
				double dWeek = 1.0 + std::floor((1.0 / 7.0) * (rd_only - firstOfYear));
				ywd[1] = static_cast<int>(dWeek);
				ywd[0] = next_year;
			}
			else if (rd_only >= (firstOfYear = FixedFromYwd( year, 1, 1)))
			{
				double dWeek = 1.0 + std::floor((1.0 / 7.0) * (rd_only - firstOfYear));
				ywd[1] = static_cast<int>(dWeek);
				ywd[0] = year;
			}
			else if (rd_only >= (firstOfYear = FixedFromYwd(prev_year, 1, 1)))
			{
				double dWeek = 1.0 + std::floor((1.0 / 7.0) * (rd_only - firstOfYear));
				ywd[1] = static_cast<int>(dWeek);
				ywd[0] = prev_year;
			}

			//double dDay = FixedDayOfWeek(rdOnly - RD0);
			double dDay = math::AFlMod(rd_only - KRD0, 7);
			ywd[2] = static_cast<int>(dDay);
			return ywd;
		}

		//==========================================================
		// Find the year a given FixedDateTime falls in
		//==========================================================
		int IsoChronology::DetermineYearFromFixed(RD rd) const
		{
			double d0 = math::ExtractDate(rd) - KISO_EPOCH;

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

			return static_cast<int>(year);
		}

		//============================================================
		// Calculate the date relative to another date
		//=============================================================
		RD IsoChronology::FixedRelativeTo(RD rd, RS rel) const
		{
			RD relative_rd = 0.0, good_rd = 0.0;
			YMD ymd = { 0,0,0 };
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

				if (ymd[1] > 12)
				{
					++ymd[0];
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

				if (ymd[1] > 12)
				{
					++ymd[0];
					ymd[1] = 1;
				}

				good_rd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				relative_rd = good_rd - 1.0;
				break;
			}

			return relative_rd;
		}

		//=====================================================
		// Calculate RD format from time fields
		//=====================================================
		RD IsoChronology::FixedFromTime(int hour, int min, int sec, int milli) const
		{
			return math::FixedFromTime(hour, min, sec, milli);
		}

		//====================================================
		// calculate time fields from RD format
		//====================================================
		HMS IsoChronology::TimeFromFixed(RD rd) const
		{
			return math::HmsFromFixed(rd);
		}

		//=============================================================================
		// calculate the week of month with min amount of days in first week
		//=============================================================================
		int IsoChronology::WeekOfMonth(const YWD& ywd, RD rd) const
		{
			int iso_week_bias = 1;
			int week;
			//Add 1 to bias for Monday as start of week
			RD first_of_month = math::GetFirstOfCycleDate(FixedFromYmd(ywd[0], ywd[1], 1), min_days_in_first_week_- 1) + iso_week_bias;
			if (rd < first_of_month)
				week = 0;
			else
			{
				RD diff = rd - first_of_month;
				week = static_cast<int>(1.0 + std::floor((1.0 / 7.0) * diff));
			}

			return week;

		}

		//=====================================================
		// Find if the given year is a leap year
		//=====================================================
		bool IsoChronology::IsLeapYear(int year) const
		{
			unsigned int abs_year = abs(year);
			const unsigned int leap100 = 100;
			const unsigned int leap400 = 400;

			if (math::FastMod4(abs_year) == 0)
			{
				if (abs_year % leap100 == 0)
				{
					if (abs_year % leap400 == 0)
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