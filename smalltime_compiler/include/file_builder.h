#pragma once
#ifndef _FILEBUILDER_
#define _FILEBUILDER_

#include <core_decls.h>
#include <tz_decls.h>
#include "comp_decls.h"

#include <vector>
#include <fstream>

namespace smalltime
{
	namespace comp
	{
		class FileBuilder
		{
		public:

			bool Build(std::vector<tz::Rule>& vec_rule, std::vector<tz::Zone>& vec_zone, std::vector<tz::Zones>& vec_zone_lookup,
				std::vector<tz::Rules>& vec_rule_lookup, std::ofstream& out_file);

		private:
			bool InsertRule(tz::Rule& rule, std::ofstream& out_file);
			bool InsertZone(tz::Zone& zone, std::ofstream& out_file);

			bool InsertRuleSearch(tz::Rules& rules, std::ofstream& out_file);
			bool InsertZoneSearch(tz::Zones& zones, std::ofstream& out_file);

		};
	}
}

#endif