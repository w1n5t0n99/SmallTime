#pragma once
#ifndef _GENERATOR_
#define _GENERATOR_

#include <CoreDecls.h>
#include <TzDecls.h>
#include <BasicDateTime.h>
#include "../CompDecls.h"

namespace smalltime
{
	namespace comp
	{
		class Generator
		{
		public:
			bool proccessUtc(std::vector<tz::Zone>& vZone);
			bool processRules(std::vector<tz::Rule>& vRule, const std::vector<RuleData>& vRuleData);
			bool processZones(std::vector<tz::Zone>& vZone, const std::vector<ZoneData>& vZoneData);
			bool processLinks(std::vector<tz::Link>& vLink, const std::vector<LinkData>& vLinkData);

		protected:

			std::array<int, 4> convertTimeStrToFields(std::string str);
			RD convertTimeStrToRd(std::string tmStr);
			BasicDateTime<> convertZoneUntil(std::string str);
			RD convertToZoneRuleOffset(std::string str);
			uint32_t convertToZoneRuleId(std::string str);
			int convertToMonth(const std::string& str);

			tz::TimeType checkTimeSuffix(const std::string& tmStr);
			tz::DayType checkDayType(const std::string& str);

		};
	}
}

#endif