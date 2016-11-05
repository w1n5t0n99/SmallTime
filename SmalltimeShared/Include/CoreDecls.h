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

	static const int RD0 = 0;
	static const RD RD_EPOCH = 1.0;

	// Used for specifying the calendar system Chronology can use
	enum class CalSpec
	{
		Iso,
		Gregorian,
		Julian,
		Islamic,
		Hebrew,
		Persian
	};

	// Used for specifying a date-time relative to another
	enum class RelSpec
	{
		SunOnOrAfter = 7,
		MonOnOrAfter = 8,
		TuesOnOrAfter = 9,
		WedOnOrAfter = 10,
		ThursOnOrAfter = 11,
		FriOnOrAfter = 12,
		SatOnOrAfter = 13,

		SunAfter = 14,
		MonAfter = 15,
		TuesAfter = 16,
		WedAfter = 17,
		ThursAfter = 18,
		FriAfter = 19,
		SatAfter = 20,

		SunOnOrBefore = 21,
		MonOnOrBefore = 22,
		TuesOnOrBefore = 23,
		WedOnOrBefore = 24,
		ThursOnOrBefore = 25,
		FriOnOrBefore = 26,
		SatOnOrBefore = 27,

		SunBefore = 28,
		MonBefore = 29,
		TuesBefore = 30,
		WedBefore = 31,
		ThursBefore = 32,
		FriBefore = 33,
		SatBefore = 34,

		LastSun = 35,
		LastMon = 36,
		LastTues = 37,
		LastWed = 38,
		LastThurs = 39,
		LastFri = 40,
		LastSat = 41,
	};

	enum class Choose
	{
		Earliest,
		Latest,
		Error
	};

}

#endif // !_CORECONSTANTS_

