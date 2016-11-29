#include "src_builder.h"
#include <core_math.h>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace smalltime
{
	namespace comp
	{
		//==============================================
		// Overload of stream operator for time type
		//==============================================
		std::ostream& operator<<(std::ostream& outStream, const tz::TimeType tmType)
		{
			if (tmType == tz::KTimeType_Wall)
				outStream << "KTimeType_Wall";
			else if(tmType == tz::KTimeType_Utc)
				outStream << "KTimeType_Utc";
			else
				outStream << "KTimeType_Std";

			return outStream;
		}

		//================================================
		// Overload of stream operator for day type
		//================================================
		std::ostream& operator<<(std::ostream& outStream, const tz::DayType day_type)
		{
			if (day_type == tz::KDayType_Dom)
				outStream << "KDayType_Dom";
			else if (day_type == tz::KDayType_LastSun)
				outStream << "KDayType_LastSun";
			else
				outStream << "KDayType_SunGE";
			
			return outStream;
		}

		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto ZONE_CMP = [](const tz::Zones& lhs, const tz::Zones& rhs)
		{
			return lhs.zone_id < rhs.zone_id;
		};

		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto RULE_CMP = [](const tz::Rules& lhs, const tz::Rules& rhs)
		{
			return lhs.rule_id < rhs.rule_id;
		};

		//===================================================
		// Add head data to file
		//====================================================
		bool SrcBuilder::BuildHead(std::ofstream& out_file)
		{
			if (!out_file)
				return false;

			// Include guards
			out_file << "#pragma once\n";
			out_file << "#ifndef _TZDB_\n";
			out_file << "#define _TZDB_\n";
			out_file << "#include \"tz_decls.h\"\n";
			out_file << "#include <cinttypes>\n";
			out_file << "#include <array>\n";
			out_file << "\nnamespace smalltime\n";
			out_file << "{\n";
			out_file << "\nnamespace tz\n";
			out_file << "{\n";

			return true;
		}

		//===================================================
		// Add tail data to file
		//====================================================
		bool SrcBuilder::BuildTail(std::ofstream& out_file)
		{
			if (!out_file)
				return false;

			out_file << "\n}\n";
			out_file << "\n}\n";
			out_file << "#endif\n";

			return true;
		}

		//=================================================
		// Add body to file
		//==================================================
		bool SrcBuilder::BuildBody(const std::vector<tz::Rule>& vec_rule, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Zones>& vec_zone_lookup,
			const std::vector<tz::Rules>& vec_rule_lookup, const MetaData& tzdb_meta, std::ofstream& out_file)
		{
			if (!out_file)
				return false;

			//buildZoneLookup(vec_zone, vec_link, vec_zone_lookup);
			//buildRuleLookup(vec_rule, vec_rule_lookup);

			// Add Meta data
			//BuildMetaData(vec_zone, vec_rule, vec_zone_lookup, vec_rule_lookup, tzdb_meta);

			out_file << "\nstatic const RD KMaxZoneOffset = " << tzdb_meta.max_zone_offset << ";";
			out_file << "\nstatic const RD KMinZoneOffset = " << tzdb_meta.min_zone_offset << ";";
			out_file << "\nstatic const RD KMaxRuleOffset = " << tzdb_meta.max_rule_offset << ";";
			out_file << "\nstatic const int KMaxZoneSize = " << tzdb_meta.max_zone_size << ";";
			out_file << "\nstatic const int KMaxRuleSize = " << tzdb_meta.max_rule_size << ";";

			out_file << "\n\nthread_local static std::array<RD, KMaxZoneSize * 3> zone_pool;";
			out_file << "\nthread_local static std::array<RD, KMaxRuleSize * 3> rule_pool;";

			out_file << "\n\nstatic constexpr std::array<Zone," << vec_zone.size() << "> KZoneArray = {\n";
			// Add zones
			for (const auto& zone : vec_zone)
				InsertZone(zone, out_file);

			out_file << "\n};\n";
			out_file << "\nstatic constexpr std::array<Rule," << vec_rule.size() << "> KRuleArray = {\n";
			// Add rules
			for (const auto& rule : vec_rule)
				InsertRule(rule, out_file);

			out_file << "\n};\n";
			out_file << "\nstatic constexpr std::array<Zones," << vec_zone_lookup.size() << "> KZoneLookupArray = {\n";
			// Add zones
			for (const auto& zones : vec_zone_lookup)
				InsertZoneSearch(zones, out_file);
			
			out_file << "\n};\n";
			out_file << "\nstatic constexpr std::array<Rules," << vec_rule_lookup.size() << "> KRuleLookupArray = {\n";
			// Add rules
			for (const auto& rules : vec_rule_lookup)
				InsertRuleSearch(rules, out_file);

			out_file << "\n};\n";

			return true;
		}

		//==================================================
		// Add single rule object into file
		//==================================================
		bool SrcBuilder::InsertRule(const tz::Rule& rule, std::ofstream& out_file)
		{
			if (!out_file)
				return false;

			// 17 digits should be enough to round trip double
			out_file << std::setprecision(17);

			out_file << "Rule {" << rule.rule_id << ", " << rule.from_year << ", " << rule.to_year << ", " << rule.month << ", "
				<< rule.day << ", " << rule.day_type << ", " << rule.at_time << ", " << rule.at_type << ", " << rule.offset << ", " << rule.letter << "}";
			out_file << ",\n";

			return true;
		}

		//==========================================================
		// Add single  zone object into file
		//============================================================
		bool SrcBuilder::InsertZone(const tz::Zone& zone, std::ofstream& out_file)
		{
			if (!out_file)
				return false;

			// 17 digits should be enough to round trip double
			out_file << std::setprecision(17);

			out_file << "Zone {" << zone.zone_id << ", " << zone.rule_id << ", " << zone.until_wall << ", " << zone.until_utc << ", " << zone.until_type << ", " << zone.zone_offset << ", " << zone.abbrev << "}";
			out_file << ",\n";

			return true;
		}

		//======================================================
		// Add single rules object into file
		//======================================================
		bool SrcBuilder::InsertRuleSearch(const tz::Rules& rules, std::ofstream& out_file)
		{
			if (!out_file)
				return false;

			out_file << "Rules {" << rules.rule_id << ", " << rules.first << ", " << rules.size << "}";
			out_file << ",\n";

			return true;
		}

		//======================================================
		// Add single zones object into file
		//======================================================
		bool SrcBuilder::InsertZoneSearch(const tz::Zones& zones, std::ofstream& out_file)
		{
			if (!out_file)
				return false;

			out_file << "Zones {" << zones.zone_id << ", " << zones.first << ", " << zones.size << "}";
			out_file << ",\n";

			return true;
		}

	}
}