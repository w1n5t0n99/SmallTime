#pragma once
#ifndef _ISOCHRONOLOGY_
#define _ISOCHRONOLOGY_

#include "core_decls.h"

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

			std::array<int, 3> YmdFromFixed(RD rd) const;
			std::array<int, 3> YwdFromFixed(RD rd) const;
			std::array<int, 2> YdFromFixed(RD rd) const;
			
			RD FixedRelativeTo(RD rd, RS rel) const;

			RD FixedFromTime(int hour, int minute, int second, int milli) const;
			std::array<int, 4> TimeFromFixed(RD rd) const;

			int WeekOfMonth(const std::array<int, 3>& ywd, RD rd) const;
			bool IsLeapYear(int year) const;

		private:
			int DetermineYearFromFixed(RD rd) const;

			const int min_days_in_first_week_ = 4;

		};

	}
}

#endif
