#include "../include/hebrew_chronology.h"
#include "../include/chrono_decls.h"
#include "../include/cal_math.h"
#include "../include/time_math.h"

namespace smalltime
{
	namespace chrono
	{
		static const RD KHEBREW_EPOCH = -1373427.0;

		//============================================================
		// Calculate the RD format from the year, month, day format
		//============================================================
		RD HebrewChronology::FixedFromYmd(int year, int month, int day) const
		{
			RD rd = 0.0;
			int mon = 0;
			int months = 0;

			months = HebrewYearMonths(year);
			rd = HebrewNewYear(year) + static_cast<RD>(day) - 1.0;

			if (month < 7) 
			{
				for (mon = 7; mon <= months; ++mon)
				{
					rd += static_cast<RD>(HebrewMonthDays(mon, year));
				}
				for (mon = 1; mon < month; ++mon)
				{
					rd += static_cast<RD>(HebrewMonthDays(mon, year));
				}
			}
			else 
			{
				for (mon = 7; mon < month; ++mon)
				{
					rd += static_cast<RD>(HebrewMonthDays(mon, year));
				}
			}

			return rd;

		}

		//=====================================================
		// Calculate the YMD format from RD format
		//=====================================================
		YMD HebrewChronology::YmdFromFixed(RD rd) const
		{
			
			int year = 0, month = 0, day = 0, count = 0, first = 0;

			RD date_only = math::ExtractDate(rd);
			date_only = std::floor(date_only);
			count = std::floor(((date_only - KHEBREW_EPOCH) * 98496.0) / 35975351.0) + 1;

			year = count - 1;
			for (int i = count; date_only >= HebrewNewYear(i); ++i) 
			{
				++year;
			}

			first = (date_only < FixedFromYmd(year, 1, 1)) ? 7 : 1;

			month = first;
			
			for (int i = first; date_only > FixedFromYmd(year, i, HebrewMonthDays(i, year)); ++i) 
			{
				++month;
			}
			

			day = 1 + static_cast<int>(date_only) - static_cast<int>(FixedFromYmd(year, month, 1));
			
			return{ year, month, day };

		}

		//============================================================
		// Calculate the date relative to another date
		//=============================================================
		RD HebrewChronology::FixedRelativeTo(RD rd, RelSpec rel) const
		{
			RD relative_rd = 0.0;
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
				auto ymd = YmdFromFixed(rd);
				// increment month
				ymd[1] += 1;
				ymd[2] = 1;

				//check if valid date and increment year if not
				auto incr_rd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				auto incr_ymd = YmdFromFixed(incr_rd);

				if (ymd[0] != incr_ymd[0] || ymd[1] != incr_ymd[1] || ymd[2] != incr_ymd[2])
				{
					ymd[0] += 1;
					ymd[1] = 1;
				}

				auto good_ymd = FixedFromYmd(ymd[0], ymd[1], ymd[2]);
				relative_rd = math::KDayBefore(dow, good_ymd);
				break;
			}

			return relative_rd;
		}

		//==========================================
		// How many days in the hebrew year
		//=============================================
		RD HebrewChronology::HebrewYearDays(int year) const
		{
			return FixedFromYmd(year + 1, KHebrewMonth_Tishri, 1) - FixedFromYmd(year, KHebrewMonth_Tishri, 1);

		}

		//============================================
		// Find the last day of given hebrew month
		//===========================================
		int HebrewChronology::HebrewMonthDays(int month, int year) const
		{
			/*
			switch (month)
			{
			case KHebrewMonth_Iyyar:
			case KHebrewMonth_Tammuz:
			case KHebrewMonth_Elul:
			case KHebrewMonth_Tevet:
			case KHebrewMonth_Adarii:
				return 29;
			case KHebrewMonth_Adar:
				if (!IsLeapYear(year))
					return 29;
			case KHebrewMonth_Marheshvan:
				if (!IsLongMarheshvan(year))
					return 29;
			case KHebrewMonth_Kislev:
				if (IsShortKislev(year))
					return 29;
			default:
				return 30;
			}
			*/

			//  First of all, dispose of fixed-length 29 day months
		
			if (month == 2 || month == 4 || month == 6 ||
				month == 10 || month == 13) 
			{
				return 29;
			}

			//  If it's not a leap year, Adar has 29 days

			if (month == 12 && !IsLeapYear(year))
			{
				return 29;
			}

			//  If it's Heshvan, days depend on length of year

			if (month == 8 && !IsLongMarheshvan(year))
			{
				return 29;
			}

			//  Similarly, Kislev varies with the length of year

			if (month == 9 && IsShortKislev(year))
			{
				return 29;
			}

			//  Nope, it's a 30 day month

			return 30;
			
		}

		//============================================
		// Find the last month of hebrew year
		//===========================================
		int HebrewChronology::HebrewYearMonths(int year) const
		{
			return IsLeapYear(year) ? KHebrewMonth_Adarii : KHebrewMonth_Adar;
		}

		//=============================================
		// Find days in year
		//=============================================
		int HebrewChronology::DaysInHebrewYear(int year) const
		{
			return HebrewNewYear(year + 1) - HebrewNewYear(year);
		}

		//============================================
		// Find if long marheshvan
		//============================================
		bool HebrewChronology::IsLongMarheshvan(int year) const
		{
			int days = DaysInHebrewYear(year);
			return( days == 355 || days == 385) ? true : false;

		}

		//============================================
		// Find if short kislev
		//============================================
		bool HebrewChronology::IsShortKislev(int year) const
		{
			int days = DaysInHebrewYear(year);
			return  (days == 353 || days == 383) ? true : false;

		}

		//===================================
		// Find hebrew new year
		//====================================
		int HebrewChronology::HebrewNewYear(int year) const
		{
			return static_cast<int>(KHEBREW_EPOCH) + HebrewDelay1(year) + HebrewDelay2(year);
		}

		//==============================================================
		//  Test for delay of start of new year and to avoid
		//  Sunday, Wednesday, and Friday as start of the new year.
		//===============================================================
		int  HebrewChronology::HebrewDelay1(int year) const
		{
			int months = 0, day = 0, parts = 0;

			months = std::floor(((235.0 * static_cast<RD>(year)) - 234.0) / 19.0);
			parts = 12084 + (13753 * months);
			day = (static_cast<RD>(months) * 29.0) + std::floor(parts / 25920.0);

			if (math::FlMod((3.0 * (static_cast<RD>(day) + 1.0)), 7.0) < 3.0)
			{
				++day;
			}

			return day;
		}

		//==============================================================
		//  Check for delay in start of new year due to length of adjacent years
		//===============================================================
		int HebrewChronology::HebrewDelay2(int year) const
		{
			int last = 0, present = 0, next = 0;

			last = HebrewDelay1(year - 1);
			present = HebrewDelay1(year);
			next = HebrewDelay1(year + 1);

			return ((next - present) == 356) ? 2 :
				(((present - last) == 382) ? 1 : 0);
		}
		
		//=====================================================
		// Find if the given year is a leap year
		//=====================================================
		bool HebrewChronology::IsLeapYear(int year) const
		{
			return math::FlMod(7.0 * (static_cast<RD>(year) + 1.0), 19.0) < 7.0;

		}

		//=====================================================
		// Calculate RD format from time fields
		//=====================================================
		RD HebrewChronology::FixedFromTime(int hour, int min, int sec, int milli) const
		{
			return math::FixedFromTime(hour, min, sec, milli);
		}

		//====================================================
		// calculate time fields from RD format
		//====================================================
		HMS HebrewChronology::TimeFromFixed(RD rd) const
		{
			return math::HmsFromFixed(rd);
		}

	}
}