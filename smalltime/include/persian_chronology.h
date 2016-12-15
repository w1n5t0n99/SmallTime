#pragma once
#ifndef _PERSIANCHRONOLOGY_
#define _PERSIANCHRONOLOGY_

#include "core_decls.h"

namespace smalltime
{

	namespace chrono
	{
		class PersianChronology
		{
		public:
			RD FixedFromYmd(int year, int month, int day) const;
			std::array<int, 3> YmdFromFixed(RD rd) const;

			RD FixedRelativeTo(RD rd, RS rel) const;

			RD FixedFromTime(int hour, int minute, int second, int milli) const;
			std::array<int, 4> TimeFromFixed(RD rd) const;

			bool IsLeapYear(int year) const;

		private:
		
			const int min_days_in_first_week_ = 4;

		};

	}
}

#endif