#include "SrcBuilder.h"
#include <MathUtils.h>
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
			if (tmType == tz::TimeType_Wall)
				outStream << "TimeType_Wall";
			else if(tmType == tz::TimeType_Utc)
				outStream << "TimeType_Utc";
			else
				outStream << "TimeType_Std";

			return outStream;
		}

		//================================================
		// Overload of stream operator for day type
		//================================================
		std::ostream& operator<<(std::ostream& outStream, const tz::DayType dayType)
		{
			if (dayType == tz::DayType_Dom)
				outStream << "DayType_Dom";
			else if (dayType == tz::DayType_LastSun)
				outStream << "DayType_LastSun";
			else
				outStream << "DayType_SunGE";
			
			return outStream;
		}

		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto ZONE_CMP = [](const tz::Zones& lhs, const tz::Zones& rhs)
		{
			return lhs.zoneId < rhs.zoneId;
		};

		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto RULE_CMP = [](const tz::Rules& lhs, const tz::Rules& rhs)
		{
			return lhs.ruleId < rhs.ruleId;
		};

		//===================================================
		// Add head data to file
		//====================================================
		bool SrcBuilder::buildHead(std::ofstream& outFile)
		{
			if (!outFile)
				return false;

			// Include guards
			outFile << "#pragma once\n";
			outFile << "#ifndef _TZDB_\n";
			outFile << "#define _TZDB_\n";
			outFile << "#include \"TzDecls.h\"\n";
			outFile << "#include <cinttypes>\n";
			outFile << "#include <array>\n";
			outFile << "\nnamespace smalltime\n";
			outFile << "{\n";
			outFile << "\nnamespace tz\n";
			outFile << "{\n";

			return true;
		}

		//===================================================
		// Add tail data to file
		//====================================================
		bool SrcBuilder::buildTail(std::ofstream& outFile)
		{
			if (!outFile)
				return false;

			outFile << "\n}\n";
			outFile << "\n}\n";
			outFile << "#endif\n";

			return true;
		}

		//=================================================
		// Add body to file
		//==================================================
		bool SrcBuilder::buildBody(const std::vector<tz::Rule>& vRules, const std::vector<tz::Zone>& vZones,
			const std::vector<tz::Link>& vLinks, std::ofstream& outFile)
		{
			if (!outFile)
				return false;

			// Create lookup data
			std::vector<tz::Zones> vLookupZones = {};
			std::vector<tz::Rules> vLookupRules = {};

			buildZoneLookup(vZones, vLinks, vLookupZones);
			buildRuleLookup(vRules, vLookupRules);

			outFile << "\nstatic constexpr std::array<Zone," << vZones.size() << "> ZONE_ARRAY = {\n";
			// Add zones
			for (const auto& zone : vZones)
				insertZone(zone, outFile);

			outFile << "\n};\n";
			outFile << "\nstatic constexpr std::array<Rule," << vRules.size() << "> RULE_ARRAY = {\n";
			// Add rules
			for (const auto& rule : vRules)
				insertRule(rule, outFile);

			outFile << "\n};\n";
			outFile << "\nstatic constexpr std::array<Zones," << vLookupZones.size() << "> ZONE_LOOKUP_ARRAY = {\n";
			// Add zones
			for (const auto& zones : vLookupZones)
				insertZoneSearch(zones, outFile);
			
			outFile << "\n};\n";
			outFile << "\nstatic constexpr std::array<Rules," << vLookupRules.size() << "> RULE_LOOKUP_ARRAY = {\n";
			// Add rules
			for (const auto& rules : vLookupRules)
				insertRuleSearch(rules, outFile);

			outFile << "\n};\n";

			return true;
		}

		//==================================================
		// Add single rule object into file
		//==================================================
		bool SrcBuilder::insertRule(const tz::Rule& rule, std::ofstream& outFile)
		{
			if (!outFile)
				return false;

			// 17 digits should be enough to round trip double
			outFile << std::setprecision(17);

			outFile << "Rule {" << rule.ruleId << ", " << rule.fromYear << ", " << rule.toYear << ", " << rule.month << ", "
				<< rule.day << ", " << rule.dayType << ", " << rule.atTime << ", " << rule.atType << ", " << rule.offset << ", " << rule.letter << "}";
			outFile << ",\n";

			return true;
		}

		//==========================================================
		// Add single  zone object into file
		//============================================================
		bool SrcBuilder::insertZone(const tz::Zone& zone, std::ofstream& outFile)
		{
			if (!outFile)
				return false;

			// 17 digits should be enough to round trip double
			outFile << std::setprecision(17);

			outFile << "Zone {" << zone.zoneId << ", " << zone.ruleId << ", " << zone.ruleOffset << ", " << zone.until << ", " << zone.untilType << ", " << zone.zoneOffset << ", " << zone.abbrev << "}";
			outFile << ",\n";

			return true;
		}

		//======================================================
		// Add single rules object into file
		//======================================================
		bool SrcBuilder::insertRuleSearch(const tz::Rules& rules, std::ofstream& outFile)
		{
			if (!outFile)
				return false;

			outFile << "Rules {" << rules.ruleId << ", " << rules.first << ", " << rules.size << "}";
			outFile << ",\n";

			return true;
		}

		//======================================================
		// Add single zones object into file
		//======================================================
		bool SrcBuilder::insertZoneSearch(const tz::Zones& zones, std::ofstream& outFile)
		{
			if (!outFile)
				return false;

			outFile << "Zones {" << zones.zoneId << ", " << zones.first << ", " << zones.size << "}";
			outFile << ",\n";

			return true;
		}

		//====================================================
		// Build sorted vector for easy zone lookup
		//====================================================
		bool SrcBuilder::buildZoneLookup(const std::vector<tz::Zone>& vZones, const std::vector<tz::Link>& vLinks, std::vector<tz::Zones>& vLookupZones)
		{
			//add zones
			if (!vZones.empty())
			{
				uint32_t curZoneId = vZones[0].zoneId;
				int firstZone = 0;
				int lastZone = 0;

				for (int i = 0; i < vZones.size(); ++i)
				{
					if (vZones[i].zoneId != curZoneId)
					{
						tz::Zones z = { curZoneId, firstZone, lastZone - firstZone + 1 };
						vLookupZones.push_back(z);

						curZoneId = vZones[i].zoneId;
						firstZone = i;
					}

					lastZone = i;
				}

				// add last set of zone entries
				tz::Zones z = { curZoneId, firstZone, lastZone - firstZone + 1 };
				vLookupZones.push_back(z);
			}
			else
				return false;

			// add links
			std::vector<tz::Zones> vLookupLinks = {};
			for (const auto& zl : vLookupZones)
			{
				for (const auto& link : vLinks)
				{
					if (link.targetZone == zl.zoneId)
					{
						tz::Zones z = { link.refZone, zl.first, zl.size };
						vLookupLinks.push_back(z);
					}
				}
			}

			// merge links back int zones
			vLookupZones.reserve(vLookupZones.size() + vLookupLinks.size());
			vLookupZones.insert(vLookupZones.end(), vLookupLinks.begin(), vLookupLinks.end());

			//sort zones by id
			std::sort(vLookupZones.begin(), vLookupZones.end(), ZONE_CMP);

			return true;
		}

		//====================================================
		// Build sorted vector for easy zone lookup
		//====================================================
		bool SrcBuilder::buildRuleLookup(const std::vector<tz::Rule>& vRules, std::vector<tz::Rules>& vLookupRules)
		{
			//add rules
			if (!vRules.empty())
			{
				uint32_t curRuleId = vRules[0].ruleId;
				int firstRule = 0;
				int lastRule = 0;

				for (int i = 0; i < vRules.size(); ++i)
				{
					if (vRules[i].ruleId != curRuleId)
					{
						tz::Rules r = { curRuleId, firstRule, lastRule - firstRule + 1 };
						vLookupRules.push_back(r);

						curRuleId = vRules[i].ruleId;
						firstRule = i;
					}

					lastRule = i;
				}

				// add last set of rule entries
				tz::Rules r = { curRuleId, firstRule, lastRule - firstRule + 1 };
				vLookupRules.push_back(r);

				//sort rules by id
				std::sort(vLookupRules.begin(), vLookupRules.end(), RULE_CMP);

				return true;
			}

			return false;
		}

	}
}