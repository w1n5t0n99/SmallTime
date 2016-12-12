#pragma once
#ifndef _HEBREWCHRONOLOGY_
#define _HEBREWCHRONOLOGY_

#include "core_decls.h"

namespace smalltime
{

	namespace chrono
	{
		class HebrewChronology
		{
		public:
			RD FixedFromYmd(int year, int month, int day) const;
			YMD YmdFromFixed(RD rd) const;


			RD FixedRelativeTo(RD rd, RelSpec rel) const;

			RD FixedFromTime(int hour, int minute, int second, int milli) const;
			HMS TimeFromFixed(RD rd) const;

			bool IsLeapYear(int year) const;

		private:
			int HebrewMonthDays(int month, int year) const;
			int HebrewYearMonths(int year) const;
			RD HebrewYearDays(int year) const;

			int HebrewDelay1(int year) const;
			int HebrewDelay2(int year) const;

			int HebrewNewYear(int year) const;
			int DaysInHebrewYear(int year) const;

			bool IsLongMarheshvan(int year) const;
			bool IsShortKislev(int year) const;

			const int min_days_in_first_week_ = 4;

			enum HebrewMonthDays
			{
				KHebrewMonth_Nisan = 1,
				KHebrewMonth_Iyyar = 2,
				KHebrewMonth_Sivan = 3,
				KHebrewMonth_Tammuz = 4,
				KHebrewMonth_Av = 5,
				KHebrewMonth_Elul = 6,
				KHebrewMonth_Tishri = 7,
				KHebrewMonth_Marheshvan = 8,
				KHebrewMonth_Kislev = 9,
				KHebrewMonth_Tevet = 10,
				KHebrewMonth_shevat = 11,
				KHebrewMonth_Adar = 12,
				KHebrewMonth_Adarii = 13
			};

		};

	}
}

#endif
