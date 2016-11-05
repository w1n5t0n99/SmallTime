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
			RD rdFromYmd(int year, int month, int day) const;
			RD rdFromYwd(int year, int month, int day) const;
			RD rdFromYd(int year, int day) const;

			YMD ymdFromRd(RD rd) const;
			YWD ywdFromRd(RD rd) const;
			YD ydFromRd(RD rd) const;
			
			RD rdRelativeTo(RD rd, RelSpec rel) const;

			RD rdFromTime(int hour, int minute, int second, int milli) const;
			HMS timeFromRd(RD rd) const;

			int weekOfMonth(const YWD& ywd, RD rd) const;
			bool isLeapYear(int year) const;

		private:
			int determineYearFromFixed(RD rd) const;

			const int m_minDaysInFirstWeek = 4;

		};

	}
}

#endif
