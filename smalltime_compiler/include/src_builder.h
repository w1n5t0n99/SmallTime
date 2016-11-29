#pragma once
#ifndef _SRCBUILDER_
#define _SRCBUILDER_

#include <core_decls.h>
#include <tz_decls.h>
#include "comp_decls.h"

#include <vector>
#include <fstream>

namespace smalltime
{
	namespace comp
	{
		class SrcBuilder
		{
		public:
			bool BuildHead(std::ofstream& out_file);
			bool BuildTail(std::ofstream& out_file);

			bool BuildBody(const std::vector<tz::Rule>& vec_rule, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Zones>& vec_zone_lookup,
				const std::vector<tz::Rules>& vec_rule_lookup, const MetaData& tzdb_meta, std::ofstream& out_file);

		private:
			bool InsertRule(const tz::Rule& rule, std::ofstream& out_file);
			bool InsertZone(const tz::Zone& zone, std::ofstream& out_file);

			bool InsertRuleSearch(const tz::Rules& rules, std::ofstream& out_file);
			bool InsertZoneSearch(const tz::Zones& zones, std::ofstream& out_file);

		};
	}
}

#endif
