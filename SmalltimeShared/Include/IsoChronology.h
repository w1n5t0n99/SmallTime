#pragma once
#ifndef _ISOCHRONOLOGY_
#define _ISOCHRONOLOGY_

#include "CoreDecls.h"

namespace smalltime
{

	namespace chrono
	{
		class IsoChronology
		{
		public:
			RD FixedFromYmd(int year, int month, int day) const;
			RD FixedFromYwd(int year, int month, int day) const;
			RD FixedFromYd(int year, int day) const;

			YMD YmdFromFixed(RD rd) const;
			YWD YwdFromFixed(RD rd) const;
			YD YdFromFixed(RD rd) const;
			
			RD FixedRelativeTo(RD rd, RelSpec rel) const;

			RD FixedFromTime(int hour, int minute, int second, int milli) const;
			HMS TimeFromFixed(RD rd) const;

			int WeekOfMonth(const YWD& ywd, RD rd) const;
			bool IsLeapYear(int year) const;

		private:
			int DetermineYearFromFixed(RD rd) const;

			const int min_days_in_first_week_ = 4;

		};

	}
}

#endif
