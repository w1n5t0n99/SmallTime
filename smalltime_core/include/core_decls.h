#pragma once
#ifndef _COREDECLS_
#define _COREDECLS_

#include <array>

namespace smalltime
{
	// The fixed date-time representation 
	// R.D = Rata Die from "Calendrical Calculations"
	using RD = double;
	// Diffrent calendar field representations
	using YMD = std::array<int, 3>;
	using YWD = std::array<int, 3>;
	using YD = std::array<int, 2>;
	using HMS = std::array<int, 4>;

	static const int KRD0 = 0;
	static const RD KRD_EPOCH = 1.0;

	// Used for specifying a date-time relative to another
	enum class RS
	{
		KSunOnOrAfter = 7,
		KMonOnOrAfter = 8,
		KTuesOnOrAfter = 9,
		KWedOnOrAfter = 10,
		KThursOnOrAfter = 11,
		KFriOnOrAfter = 12,
		KSatOnOrAfter = 13,

		KSunAfter = 14,
		KMonAfter = 15,
		KTuesAfter = 16,
		KWedAfter = 17,
		KThursAfter = 18,
		KFriAfter = 19,
		KSatAfter = 20,

		KSunOnOrBefore = 21,
		KMonOnOrBefore = 22,
		KTuesOnOrBefore = 23,
		KWedOnOrBefore = 24,
		KThursOnOrBefore = 25,
		KFriOnOrBefore = 26,
		KSatOnOrBefore = 27,

		KSunBefore = 28,
		KMonBefore = 29,
		KTuesBefore = 30,
		KWedBefore = 31,
		KThursBefore = 32,
		KFriBefore = 33,
		KSatBefore = 34,

		KLastSun = 35,
		KLastMon = 36,
		KLastTues = 37,
		KLastWed = 38,
		KLastThurs = 39,
		KLastFri = 40,
		KLastSat = 41,

		KLastDay = 42,
	};

	enum class Choose
	{
		KEarliest,
		KLatest,
		KError
	};

}

#endif // !_CORECONSTANTS_

