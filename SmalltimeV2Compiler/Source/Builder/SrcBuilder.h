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

			bool buildBody(const std::vector<tz::Rule>& vec_rule, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Zones>& vec_zone_lookup,
				const std::vector<tz::Rules>& vec_rule_lookup, const MetaData& tzdb_meta, std::ofstream& out_file);

		private:
			bool insertRule(const tz::Rule& rule, std::ofstream& outFile);
			bool insertZone(const tz::Zone& zone, std::ofstream& outFile);

			bool insertRuleSearch(const tz::Rules& rules, std::ofstream& outFile);
			bool insertZoneSearch(const tz::Zones& zones, std::ofstream& outFile);

		};
	}
}

#endif
