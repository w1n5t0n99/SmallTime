#pragma once
#ifndef _ISLAMICCHRONOLOGY_
#define _ISLAMICCHRONOLOGY_

#include "core_decls.h"

namespace smalltime
{

	namespace chrono
	{
		class IslamicChronology
		{
		public:
			RD FixedFromYmd(int year, int month, int day) const;
			YMD YmdFromFixed(RD rd) const;


			RD FixedRelativeTo(RD rd, RS rel) const;

			RD FixedFromTime(int hour, int minute, int second, int milli) const;
			HMS TimeFromFixed(RD rd) const;

			bool IsLeapYear(int year) const;

		private:
			int DetermineYearFromFixed(RD rd) const;

			const int min_days_in_first_week_ = 4;

		};

	}
}

#endif
