#pragma once
#ifndef _GENERATOR_
#define _GENERATOR_

#include <core_decls.h>
#include <tz_decls.h>
#include <basic_datetime.h>
#include <tzdb_connector_interface.h>
#include <memory>
#include "comp_decls.h"

namespace smalltime
{
	namespace comp
	{
		class Generator
		{
		public:
			bool ProccessUtc(std::vector<tz::Zone>& vec_zone);
			bool ProcessRules(std::vector<tz::Rule>& vec_rule, const std::vector<RuleData>& vec_ruledata);
			bool ProcessZones(std::vector<tz::Zone>& vec_zone, const std::vector<ZoneData>& vec_zonedata);
			bool ProcessLinks(std::vector<tz::Link>& vec_link, const std::vector<LinkData>& vec_linkdata);

			bool ProcessZoneLookup(std::vector<tz::Zones>& vec_zone_lookup, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Link>& vec_link);
			bool ProcessRuleLookup(std::vector<tz::Rules>& vec_rule_lookup, const std::vector<tz::Rule>& vec_rule);
			bool ProcessMeta(MetaData& tzdb_meta, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Rule>& vec_rule,
				const std::vector<tz::Zones>& vec_zone_lookup, const std::vector<tz::Rules>& vec_rule_lookup);

		protected:

			std::array<int, 4> ConvertTimeStrToFields(std::string str);
			RD ConvertTimeStrToRd(std::string time_str);
			BasicDateTime<> ConvertZoneUntil(std::string str);
			RD ConvertToZoneRuleOffset(std::string str);
			uint32_t ConvertToZoneRuleId(std::string str);
			int ConvertToMonth(const std::string& str);

			tz::TimeType CheckTimeSuffix(const std::string& time_str);
			tz::DayType CheckDayType(const std::string& str);

		};
	}
}

#endif