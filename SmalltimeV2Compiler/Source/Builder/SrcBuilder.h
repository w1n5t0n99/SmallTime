#pragma once
#ifndef _SRCBUILDER_
#define _SRCBUILDER_

#include <CoreDecls.h>
#include <TzDecls.h>
#include "../CompDecls.h"

#include <vector>
#include <fstream>

namespace smalltime
{
	namespace comp
	{
		class SrcBuilder
		{
		public:
			bool buildHead(std::ofstream& outFile);
			bool buildTail(std::ofstream& outFile);

			bool buildBody(const std::vector<tz::Rule>& vRules, const std::vector<tz::Zone>& vZones,
				const std::vector<tz::Link>& vLinks, std::ofstream& outFile);

		private:
			bool buildZoneLookup(const std::vector<tz::Zone>& vZones, const std::vector<tz::Link>& vLinks, std::vector<tz::Zones>& vLookupZones);
			bool buildRuleLookup(const std::vector<tz::Rule>& vRules, std::vector<tz::Rules>& vLookupRules);

			bool BuildMetaData(const std::vector<tz::Zone>& vZone, const std::vector<tz::Rule>& vRule, const std::vector<tz::Zones>& vLookupZones, const std::vector<tz::Rules>& vLookupRules, MetaData& mdata);

			bool insertRule(const tz::Rule& rule, std::ofstream& outFile);
			bool insertZone(const tz::Zone& zone, std::ofstream& outFile);

			bool insertRuleSearch(const tz::Rules& rules, std::ofstream& outFile);
			bool insertZoneSearch(const tz::Zones& zones, std::ofstream& outFile);

		};
	}
}

#endif
