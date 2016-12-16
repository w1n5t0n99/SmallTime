#include "../include/file_builder.h"
#include <core_math.h>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace smalltime
{
	namespace comp
	{
		//========================================
		// Build binary file of tzdb data
		//========================================
		bool FileBuilder::Build(std::vector<tz::Rule>& vec_rule, std::vector<tz::Zone>& vec_zone, std::vector<tz::Zones>& vec_zone_lookup,
			std::vector<tz::Rules>& vec_rule_lookup, std::ofstream& out_file)
		{

			out_file.seekp(out_file.beg);
			auto tzdb_id = math::GetUniqueID("TZDB_FILE");
			//simple file corruption check
			tz::Zone z;
			int total_zone_size = (sizeof(z.abbrev) + sizeof(z.mb_rule_offset) + sizeof(z.mb_until_utc) + sizeof(z.next_zone_offset) +
				sizeof(z.rule_id) + sizeof(z.trans_rule_offset) + sizeof(z.until_type) + sizeof(z.zone_id) + sizeof(z.zone_offset)) * vec_zone.size();

			tz::Rule r;
			int total_rule_size = (sizeof(r.at_time) + sizeof(r.at_type) + sizeof(r.day) + sizeof(r.day_type) +
				sizeof(r.from_year) + sizeof(r.letter) + sizeof(r.month) + sizeof(r.offset) + sizeof(r.rule_id) + sizeof(r.to_year)) * vec_rule.size();

			tz::Zones zs;
			int total_zones_size = (sizeof(zs.first) + sizeof(zs.size) + sizeof(zs.zone_id)) * vec_zone_lookup.size();


			tz::Zones rs;
			int total_rules_size = (sizeof(rs.first) + sizeof(rs.size) + sizeof(rs.zone_id)) * vec_rule_lookup.size();

			int tzdb_file_size = (sizeof(int) * 5) + sizeof(tzdb_id) + total_zone_size + total_rule_size + total_zones_size + total_rules_size;

			out_file.write(reinterpret_cast<char*>(&tzdb_id), sizeof(tzdb_id));
			out_file.write(reinterpret_cast<char*>(&tzdb_file_size), sizeof(tzdb_file_size));

			// write zones
			out_file.write(reinterpret_cast<char*>(&total_zone_size), sizeof(total_zone_size));
			for (auto& z : vec_zone)
				InsertZone(z, out_file);
			// write rules
			out_file.write(reinterpret_cast<char*>(&total_rule_size), sizeof(total_rule_size));
			for (auto& r : vec_rule)
				InsertRule(r, out_file);
			// write zone lookup
			out_file.write(reinterpret_cast<char*>(&total_zones_size), sizeof(total_zones_size));
			for (auto& zs : vec_zone_lookup)
				InsertZoneSearch(zs, out_file);
			// write rule lookup
			out_file.write(reinterpret_cast<char*>(&total_rules_size), sizeof(total_rules_size));
			for (auto& rs : vec_rule_lookup)
				InsertRuleSearch(rs, out_file);

			return true;
		}

		//==================================================
		// Convert and insert zone as binary format
		//===================================================
		bool FileBuilder::InsertZone(tz::Zone& zone, std::ofstream& out_file)
		{
			out_file.write(reinterpret_cast<char*>(&zone.zone_id), sizeof(zone.zone_id));
			out_file.write(reinterpret_cast<char*>(&zone.rule_id), sizeof(zone.rule_id));
			out_file.write(reinterpret_cast<char*>(&zone.mb_until_utc), sizeof(zone.mb_until_utc));
			out_file.write(reinterpret_cast<char*>(&zone.until_type), sizeof(zone.until_type));
			out_file.write(reinterpret_cast<char*>(&zone.zone_offset), sizeof(zone.zone_offset));
			out_file.write(reinterpret_cast<char*>(&zone.next_zone_offset), sizeof(zone.next_zone_offset));
			out_file.write(reinterpret_cast<char*>(&zone.mb_rule_offset), sizeof(zone.mb_rule_offset));
			out_file.write(reinterpret_cast<char*>(&zone.trans_rule_offset), sizeof(zone.trans_rule_offset));
			out_file.write(reinterpret_cast<char*>(&zone.abbrev), sizeof(zone.abbrev));

			return true;
		}

		//=========================================================
		// convert and insert rule as binary format
		//========================================================
		bool FileBuilder::InsertRule(tz::Rule& rule, std::ofstream& out_file)
		{
			out_file.write(reinterpret_cast<char*>(&rule.rule_id), sizeof(rule.rule_id));
			out_file.write(reinterpret_cast<char*>(&rule.from_year), sizeof(rule.from_year));
			out_file.write(reinterpret_cast<char*>(&rule.to_year), sizeof(rule.to_year));
			out_file.write(reinterpret_cast<char*>(&rule.month), sizeof(rule.month));
			out_file.write(reinterpret_cast<char*>(&rule.day), sizeof(rule.day));
			out_file.write(reinterpret_cast<char*>(&rule.day_type), sizeof(rule.day_type));
			out_file.write(reinterpret_cast<char*>(&rule.at_time), sizeof(rule.at_time));
			out_file.write(reinterpret_cast<char*>(&rule.at_type), sizeof(rule.at_type));
			out_file.write(reinterpret_cast<char*>(&rule.offset), sizeof(rule.offset));
			out_file.write(reinterpret_cast<char*>(&rule.letter), sizeof(rule.letter));

			return true;
		}

		//==================================================
		// Convert and insert zone lookup as binary format
		//===================================================
		bool FileBuilder::InsertZoneSearch(tz::Zones& zones, std::ofstream& out_file)
		{
			out_file.write(reinterpret_cast<char*>(&zones.zone_id), sizeof(zones.zone_id));
			out_file.write(reinterpret_cast<char*>(&zones.first), sizeof(zones.first));
			out_file.write(reinterpret_cast<char*>(&zones.size), sizeof(zones.size));
			
			return true;
		}

		//==================================================
		// Convert and insert rule lookup as binary format
		//===================================================
		bool FileBuilder::InsertRuleSearch(tz::Rules& rules, std::ofstream& out_file)
		{
			out_file.write(reinterpret_cast<char*>(&rules.rule_id), sizeof(rules.rule_id));
			out_file.write(reinterpret_cast<char*>(&rules.first), sizeof(rules.first));
			out_file.write(reinterpret_cast<char*>(&rules.size), sizeof(rules.size));

			return true;
		}

	}
}