#pragma once
#ifndef _TZDECLS_
#define _TZDECLS_

#include "core_decls.h"
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
			KTimeType_Wall = 0x1,
			KTimeType_Std = 0x2,
			KTimeType_Utc = 0x4
		};

		enum DayType
		{
			KDayType_SunGE = 0,
			KDayType_LastSun = 1,
			KDayType_Dom = 2
		};

		enum RuleType
		{
			KRuleType_Offset = 0,
			KRuleType_None = 1,
			KRuleType_ID = 2
		};

		struct Rule
		{
			uint32_t rule_id;
			int from_year;
			int to_year;
			int month;
			int day;
			DayType day_type;
			RD at_time;
			TimeType at_type;
			RD offset;
			uint32_t letter;
		};

		struct Zone
		{
			uint32_t zone_id;
			uint32_t rule_id;
			RD until_wall;
			RD until_utc;
			RD until_diff;
			TimeType until_type;
			RD zone_offset;
			uint64_t abbrev;
		};

		struct Link
		{
			uint32_t ref_zone;
			uint32_t target_zone;
		};

		struct Zones
		{
			uint32_t zone_id;
			int first;
			int size;
		};

		struct Rules
		{
			uint32_t rule_id;
			int first;
			int size;
		};


	}
}

#endif
