#pragma once
#ifndef _TZDECLS_
#define _TZDECLS_

#include "CoreDecls.h"
#include <cinttypes>

namespace smalltime
{
	namespace tz
	{
		static const int ONLY = -999;
		static const RD DMAX = 3651695.0; // (9999/1/1)
		static const int MAX = 9999;
		
		enum TimeType
		{
			TimeType_Wall = 0x1,
			TimeType_Std = 0x2,
			TimeType_Utc = 0x4
		};

		enum DayType
		{
			DayType_SunGE = 0,
			DayType_LastSun = 1,
			DayType_Dom = 2
		};

		enum RuleType
		{
			RuleType_Offset = 0,
			RuleType_None = 1,
			RuleType_ID = 2
		};

		struct Rule
		{
			uint32_t ruleId;
			int fromYear;
			int toYear;
			int month;
			int day;
			DayType dayType;
			RD atTime;
			TimeType atType;
			RD offset;
			uint32_t letter;
		};

		struct Zone
		{
			uint32_t zoneId;
			uint32_t ruleId;
			RD ruleOffset;
			RD until;
			TimeType untilType;
			RD zoneOffset;
			uint64_t abbrev;
		};

		struct Link
		{
			uint32_t refZone;
			uint32_t targetZone;
		};

		struct Zones
		{
			uint32_t zoneId;
			int first;
			int size;
		};

		struct Rules
		{
			uint32_t ruleId;
			int first;
			int size;
		};

	}
}

#endif
