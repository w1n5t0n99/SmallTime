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

			RD FixedRelativeTo(RD rd, RS rel) const;

			RD FixedFromTime(int hour, int minute, int second, int milli) const;
			HMS TimeFromFixed(RD rd) const;

			bool IsLeapYear(int year) const;

		private:
			int HebrewMonthDays(int month, int year) const;
			int HebrewYearMonths(int year) const;
			RD HebrewYearDays(int year) const;

			RD HebrewDelay1(int year) const;
			RD HebrewDelay2(int year) const;

			RD HebrewNewYear(int year) const;

			bool IsLongMarheshvan(int year) const;
			bool IsShortKislev(int year) const;

			const int min_days_in_first_week_ = 4;

		};

	}
}

#endif
