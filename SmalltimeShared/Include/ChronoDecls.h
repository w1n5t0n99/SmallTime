#pragma once
#ifndef _CHRONODECLS_
#define _CHRONODECLS_

#include "CoreDecls.h"
#include <array>

namespace smalltime
{
	namespace chrono
	{
		static const RD ISO_EPOCH = 1.0;

		namespace iso_greg_julian
		{
			// Initiliaze static members
			static constexpr std::array<int, 12> STANDARD_MONTHS{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			static constexpr std::array<int, 12> LEAPYEAR_MONTHS{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

			//-----------------------------------------------------------
			// Pre cache of the Calendrical Calculations algorithms
			//-----------------------------------------------------------
			//  STD_MONTH_BIAS = calmath::FlMod(date.month - 3.0, 12.0)
			static constexpr std::array<int, 12> STD_MONTH_BIAS{ 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			//  STD_BASE_MONTH_Days = std::floor((2.0 + 3.0 * (calmath::FlMod(date.month - 3.0, 12.0)) / 5.0)
			static constexpr std::array<int, 12> STD_BASE_MONTH_DAYS{ 6, 7, 0, 1, 1, 2, 2, 3, 4, 4, 5, 5 };
			//	OPT_BASE_MONTH_Days = std::floor((1.0 / 12.0) * (367 * dMonth - 362))
			static constexpr std::array<int, 12> OPT_BASE_MONTH_DAYS{ 0, 31, 61, 92, 122, 153, 183, 214, 245, 275, 306, 336 };
			//	STD_YEAR_BIAS = Floor(STD_MONTH_BIAS[month - 1] / 10.0)
			static const std::array<double, 12> STD_YEAR_BIAS
			{
				std::floor(STD_MONTH_BIAS[0] / 10.0),
				std::floor(STD_MONTH_BIAS[1] / 10.0),
				std::floor(STD_MONTH_BIAS[2] / 10.0),
				std::floor(STD_MONTH_BIAS[3] / 10.0),
				std::floor(STD_MONTH_BIAS[4] / 10.0),
				std::floor(STD_MONTH_BIAS[5] / 10.0),
				std::floor(STD_MONTH_BIAS[6] / 10.0),
				std::floor(STD_MONTH_BIAS[7] / 10.0),
				std::floor(STD_MONTH_BIAS[8] / 10.0),
				std::floor(STD_MONTH_BIAS[9] / 10.0),
				std::floor(STD_MONTH_BIAS[10] / 10.0),
				std::floor(STD_MONTH_BIAS[11] / 10.0)
			};
			// YEAR_CACHE = Floor((month + 9.0) / 12.0)
			static const std::array<double, 12> YEAR_CACHE
			{
				std::floor((1 + 9.0) / 12.0),
				std::floor((2 + 9.0) / 12.0),
				std::floor((3 + 9.0) / 12.0),
				std::floor((4 + 9.0) / 12.0),
				std::floor((5 + 9.0) / 12.0),
				std::floor((6 + 9.0) / 12.0),
				std::floor((7 + 9.0) / 12.0),
				std::floor((8 + 9.0) / 12.0),
				std::floor((9 + 9.0) / 12.0),
				std::floor((10 + 9.0) / 12.0),
				std::floor((11 + 9.0) / 12.0),
				std::floor((12 + 9.0) / 12.0)
			};

		}
	}
}

#endif