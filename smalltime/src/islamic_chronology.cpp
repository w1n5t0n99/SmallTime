#include "../include/islamic_chronology.h"
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

		static const RD KISLAMIC_EPOCH = 227015.0;

		//============================================================
		// Calculate the RD format from the year, month, day format
		//============================================================
		RD IslamicChronology::FixedFromYmd(int year, int month, int day) const
		{
			RD d_year = static_cast<RD>(year);
			RD d_month = static_cast<RD>(month);
			RD d_day = static_cast<RD>(day);

			RD base = KISLAMIC_EPOCH - 1.0 + ((d_year - 1) * 354.0);
			RD yearPortion = std::floor((1.0 / 30.0) * (3.0 + (11.0 * d_year)));
			RD monthPortion = 29.0 * (d_month - 1.0);
			RD dayPortion = std::floor(d_month / 2.0) + d_day;

			return base + yearPortion + monthPortion + dayPortion;
		}

		//=====================================================
		// Calculate the YMD format from RD format
		//=====================================================
		YMD IslamicChronology::YmdFromFixed(RD rd) const
		{
			RD date_only = math::ExtractDate(rd);

			int year = static_cast<int>(std::floor((1.0 / 10631.0) * (30.0 * (date_only - KISLAMIC_EPOCH) + 10646.0)));

			RD priorDays = date_only - FixedFromYmd(year, 1, 1);
			int month = static_cast<int>(std::floor((1.0 / 325.0) * (11.0 * priorDays + 330.0)));

			int day = static_cast<int>(1.0 + date_only - FixedFromYmd(year, month, 1));

			return{ year, month, day };
		}

		//==========================================================
		// Find the year a given FixedDateTime falls in
		//==========================================================
		int IslamicChronology::DetermineYearFromFixed(RD rd) const
		{
			RD date_only = math::ExtractDate(rd);
			int year = static_cast<int>(std::floor((1.0 / 10631.0) * (30.0 * (date_only - KISLAMIC_EPOCH) + 10646.0)));
			return year;
		}

		//============================================================
		// Calculate the date relative to another date
		//=============================================================
		RD IslamicChronology::FixedRelativeTo(RD rd, RS rel) const
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

				good_rd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				relative_rd = good_rd - 1.0;
				break;
			}

			return relative_rd;
		}

		//=====================================================
		// Calculate RD format from time fields
		//=====================================================
		RD IslamicChronology::FixedFromTime(int hour, int min, int sec, int milli) const
		{
			return math::FixedFromTime(hour, min, sec, milli);
		}

		//====================================================
		// calculate time fields from RD format
		//====================================================
		HMS IslamicChronology::TimeFromFixed(RD rd) const
		{
			return math::HmsFromFixed(rd);
		}

		//=====================================================
		// Find if the given year is a leap year
		//=====================================================
		bool IslamicChronology::IsLeapYear(int year) const
		{
			RD d_year = static_cast<RD>(year);
			if (math::FlMod((14.0 + 11.0 * d_year), 30.0) < 11.0)
				return true;
			else
				return false;
		}

	}
}