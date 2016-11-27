#include "Generator.h"
#include <BasicDateTime.h>
#include <MathUtils.h>
#include <TimeMath.h>
#include <sstream>
#include <array>
#include <iomanip>
#include <iostream>
#include <rule_group.h>

namespace smalltime
{
	namespace comp
	{
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

		//============================================
		// Add a utc zone entry to list of zones
		//============================================
		bool Generator::ProccessUtc(std::vector<tz::Zone>& vec_zone)
		{
			// Set a UTC entry
			tz::Zone utcZone = { math::getUniqueID("UTC") , 0, 0.0, tz::DMAX, tz::KTimeType_Wall, 0.0, math::pack8Chars("UTC") };
			vec_zone.push_back(utcZone);

			return true;
		}

		//====================================================================================
		// Convert rule tokens to intermediate data, returns amount of rules processed
		//=======================================================================================
		bool Generator::ProcessRules(std::vector<tz::Rule>& vec_rule, const std::vector<RuleData>& vec_ruledata)
		{
			//iterate through all rule tokens and convert
			for (const auto& ruleData : vec_ruledata)
			{
				tz::Rule rule;
				rule.rule_id = math::getUniqueID(ruleData.name);
				rule.from_year = atoi(ruleData.from.c_str());

				//check if thier is  a "TO" year
				if (ruleData.to == "only")
					rule.to_year = rule.from_year;
				else if (ruleData.to == "max")
					rule.to_year = tz::MAX;
				else
					rule.to_year = atoi(ruleData.to.c_str());

				// Month
				rule.month = ConvertToMonth(ruleData.in);

				//check if "ON" is day of month or not
				rule.day_type = CheckDayType(ruleData.on);
				//rule.onType = onType;
				if (rule.day_type == tz::KDayType_Dom)
					rule.day = atoi(ruleData.on.c_str());
				else if (rule.day_type == tz::KDayType_SunGE)
					rule.day = atoi(ruleData.on.substr(5, ruleData.on.size() - 1).c_str());
				else if (rule.day_type == tz::KDayType_LastSun)
					rule.day = 0;
				else
					rule.from_year = 0;

				//check if "AT" is wall clock or something else
				rule.at_type = CheckTimeSuffix(ruleData.at);
				rule.at_time = ConvertTimeStrToRd(ruleData.at);

				rule.offset = ConvertTimeStrToRd(ruleData.save);
				rule.letter = math::pack4Chars(ruleData.letters);

				vec_rule.push_back(rule);
			}

			return true;

		}

		//======================================================================
		// Convert zone tokens into intermediate data
		//======================================================================
		bool Generator::ProcessZones(std::vector<tz::Zone>& vec_zone, const std::vector<ZoneData>& vec_zonedata)
		{
			for (const auto& zl : vec_zonedata)
			{
				tz::Zone fzl;
				fzl.zone_id = math::getUniqueID(zl.name);
				//	fzl.rule = HashString(zl.rule);   //TODO: Rule can also be null "-" or a time offser e.g. 2:00

				fzl.rule_id = ConvertToZoneRuleId(zl.rule);
				fzl.until_utc = ConvertToZoneRuleOffset(zl.rule);

				fzl.zone_offset = ConvertTimeStrToRd(zl.gmt_offset);
				fzl.abbrev = math::pack8Chars(zl.format);

				auto untilDt = ConvertZoneUntil(zl.until);
				fzl.until_wall = untilDt.getRd();
				fzl.until_type = untilDt.getType();

				vec_zone.push_back(fzl);

			}

			return true;
		}

		//======================================================================
		// Convert link tokens into intermediate data
		//======================================================================
		bool Generator::ProcessLinks(std::vector<tz::Link>& vec_link, const std::vector<LinkData>& vec_linkdata)
		{
			for (const auto& ll : vec_linkdata)
			{
				tz::Link fll;

				fll.ref_zone = math::getUniqueID(ll.ref_zone_name);
				fll.target_zone = math::getUniqueID(ll.target_zone_name);
				vec_link.push_back(fll);

			}

			return true;
		}

		//====================================================================
		// Process lookup vector from zone data
		//====================================================================
		bool Generator::ProcessZoneLookup(std::vector<tz::Zones>& vec_zone_lookup, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Link>& vec_link)
		{
			//add zones
			if (!vec_zone.empty())
			{
				uint32_t curZoneId = vec_zone[0].zone_id;
				int firstZone = 0;
				int lastZone = 0;

				for (int i = 0; i < vec_zone.size(); ++i)
				{
					if (vec_zone[i].zone_id != curZoneId)
					{
						tz::Zones z = { curZoneId, firstZone, lastZone - firstZone + 1 };
						vec_zone_lookup.push_back(z);

						curZoneId = vec_zone[i].zone_id;
						firstZone = i;
					}

					lastZone = i;
				}

				// add last set of zone entries
				tz::Zones z = { curZoneId, firstZone, lastZone - firstZone + 1 };
				vec_zone_lookup.push_back(z);
			}
			else
			{
				return false;
			}

			// add links
			std::vector<tz::Zones> vec_link_lookup = {};
			for (const auto& zl : vec_link_lookup)
			{
				for (const auto& link : vec_link)
				{
					if (link.target_zone == zl.zone_id)
					{
						tz::Zones z = { link.ref_zone, zl.first, zl.size };
						vec_link_lookup.push_back(z);
					}
				}
			}

			// merge links back int zones
			vec_zone_lookup.reserve(vec_zone_lookup.size() + vec_link_lookup.size());
			vec_zone_lookup.insert(vec_zone_lookup.end(), vec_link_lookup.begin(), vec_link_lookup.end());

			//sort zones by id
			std::sort(vec_zone_lookup.begin(), vec_zone_lookup.end(), ZONE_CMP);

			return true;
		}

		//====================================================================
		// Process lookup vector from rule data
		//====================================================================
		bool Generator::ProcessRuleLookup(std::vector<tz::Rules>& vec_rule_lookup, const std::vector<tz::Rule>& vec_rule)
		{
			//add rules
			if (!vec_rule.empty())
			{
				uint32_t curRuleId = vec_rule[0].rule_id;
				int firstRule = 0;
				int lastRule = 0;

				for (int i = 0; i < vec_rule.size(); ++i)
				{
					if (vec_rule[i].rule_id != curRuleId)
					{
						tz::Rules r = { curRuleId, firstRule, lastRule - firstRule + 1 };
						vec_rule_lookup.push_back(r);

						curRuleId = vec_rule[i].rule_id;
						firstRule = i;
					}

					lastRule = i;
				}

				// add last set of rule entries
				tz::Rules r = { curRuleId, firstRule, lastRule - firstRule + 1 };
				vec_rule_lookup.push_back(r);

				//sort rules by id
				std::sort(vec_rule_lookup.begin(), vec_rule_lookup.end(), RULE_CMP);

				return true;
			}

			return false;
		}

		//====================================================================
		// Process meta data from zone and rule data
		//====================================================================
		bool Generator::ProcessMeta(MetaData& tzdb_meta, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Rule>& vec_rule,
			const std::vector<tz::Zones>& vec_zone_lookup, const std::vector<tz::Rules>& vec_rule_lookup)
		{
			tzdb_meta.max_zone_offset = 0.0; tzdb_meta.min_zone_offset = 0.0; tzdb_meta.max_rule_offset = 0.0;
			// find min and max zone offset
			for (const auto& zone : vec_zone)
			{
				if (zone.zone_offset < tzdb_meta.min_zone_offset)
					tzdb_meta.min_zone_offset = zone.zone_offset;
				else if (zone.zone_offset > tzdb_meta.max_zone_offset)
					tzdb_meta.max_zone_offset = zone.zone_offset;
			}

			// find the max rule offset, min is always 0.0
			for (const auto& rule : vec_rule)
			{
				if (rule.offset > tzdb_meta.max_rule_offset)
					tzdb_meta.max_rule_offset = rule.offset;
			}

			tzdb_meta.max_zone_size = 1; tzdb_meta.max_rule_size = 1;
			// find the max zone size
			for (const auto& zones : vec_zone_lookup)
			{
				if (zones.size > tzdb_meta.max_zone_size)
					tzdb_meta.max_zone_size = zones.size;
			}

			// find the max rule size
			for (const auto& rules : vec_rule_lookup)
			{
				if (rules.size > tzdb_meta.max_rule_size)
					tzdb_meta.max_rule_size = rules.size;
			}

			return true;
		}

		//=======================================================
		// Process zone transition times
		//=======================================================
		bool Generator::PostProcessZones(std::vector<tz::Zone>& vec_zone, std::shared_ptr<tz::ITzdbConnector> tzdb_connector)
		{
			// util_wall stores temp until 
			//until_utc stores temp rule offset
			RD utc = 0.0;
			RD wall = 0.0;
			for (auto& z : vec_zone)
			{
				// create wall transition
				if (z.until_type == tz::KTimeType_Wall)
				{
					wall = z.until_wall - math::MSEC();

					RD rule_offset = z.until_utc;
					if (z.rule_id != 0)
					{
						try
						{
							tz::RuleGroup rg(z.rule_id, &z, tzdb_connector);
							auto r = rg.FindActiveRule(BasicDateTime<>(wall, tz::KTimeType_Wall), Choose::Latest);
							if (r)
								rule_offset = r->offset;
						}
						catch (const std::exception& e)
						{
							
							std::cout << e.what() << std::endl;
						}
					}

					utc = wall - z.zone_offset - rule_offset;
				}
				else if (z.until_type == tz::KTimeType_Std)
				{
					utc = z.until_wall - z.zone_offset - math::MSEC();

					RD rule_offset = z.until_utc;
					if (z.rule_id != 0)
					{
						tz::RuleGroup rg(z.rule_id, &z, tzdb_connector);
						auto r = rg.FindActiveRule(BasicDateTime<>(utc, tz::KTimeType_Utc), Choose::Latest);
						if (r)
							rule_offset = r->offset;
					}

					wall = z.until_wall + rule_offset;
				}
				else
				{
					utc = z.until_wall - math::MSEC();

					RD rule_offset = z.until_utc;
					if (z.rule_id != 0)
					{
						tz::RuleGroup rg(z.rule_id, &z, tzdb_connector);
						auto r = rg.FindActiveRule(BasicDateTime<>(utc, tz::KTimeType_Utc), Choose::Latest);
						if (r)
							rule_offset = r->offset;
					}

					wall = z.until_wall + z.zone_offset + rule_offset;
				}

				z.until_wall = wall;
				z.until_utc = utc;
			}

			return true;
		}

		//============================================================
		// Convert date/time string into integer data
		//==============================================================
		std::array<int, 4> Generator::ConvertTimeStrToFields(std::string str)
		{
			if (str.empty())
				return{ -1, -1 , -1, -1 };

			//replace : delimeter with space
			std::replace(str.begin(), str.end(), ':', ' ');

			char lastChar = str.back();
			//check if time does have suffix
			if (lastChar == 'w' || lastChar == 's' || lastChar == 'u' || lastChar == 'z' || lastChar == 'g')
				str.pop_back();

			std::stringstream sstr(str);
			std::string tempStr;

			std::array<int, 4> retArray = { 0, 0, 0, 0 };

			int i = 0;
			bool firstField = true, pos = true;
			while (sstr >> tempStr)
			{
				//Check if negative or positive
				if (firstField && (tempStr.find("-") != std::string::npos))
					pos = false;


				int num = atoi(tempStr.c_str());

				//time only has max millisecond accuracy
				if (i >= 4)
					return{ -1, -1 , -1, -1 };

				retArray[i] = num;
				++i;
				firstField = false;
			}

			// the tzdb only only adds negative sign to very first field
			// we need to apply the negative to all integral fields for proper calculation
			if (pos)
				return{ retArray[0], retArray[1], retArray[2], retArray[3] };
			else
				return{ retArray[0], -retArray[1], -retArray[2], -retArray[3] };

		}

		//=========================================================
		// Convert time string to fixed
		//=======================================================
		RD Generator::ConvertTimeStrToRd(std::string tmStr)
		{
			auto tp = ConvertTimeStrToFields(tmStr);

			return math::rdFromTime(tp[0], tp[1], tp[2], tp[3]);
		}

		//==========================================================================
		// Convert "Until" zone data to a LocalDateTime to obtain FixedPoint value
		//==========================================================================
		BasicDateTime<> Generator::ConvertZoneUntil(std::string str)
		{
			// if string is empty or only contains whitespace
			// characters then must be a continuation line
			if (str.empty() || str.find_first_not_of(' \t\n') == std::string::npos)
				return BasicDateTime<>(tz::DMAX, tz::KTimeType_Wall);

			std::stringstream sstr(str);
			std::string tempStr;

			int field = 0;
			std::array<int, 3> cp = { 1, 1, 1 };
			std::array<int, 4> tp = { 0, 0, 0, 0 };
			tz::TimeType timeType = tz::KTimeType_Wall;
			tz::DayType onType = tz::KDayType_Dom;

			while (sstr >> tempStr)
			{
				switch (field)
				{
				case 0:
					//year
					cp[0] = atoi(tempStr.c_str());
					break;
				case 1:
					//month
					cp[1] = ConvertToMonth(tempStr);
					break;
				case 2:
					//day - zones dont use Sun>=
					if (tempStr == "lastSun")
						onType = tz::KDayType_LastSun;

					cp[2] = atoi(tempStr.c_str());
					break;
				case 3:
					//time
					timeType = CheckTimeSuffix(tempStr);
					tp = ConvertTimeStrToFields(tempStr);   //If hour is 24, it should loop to 0 but need to error check!!
					break;
				}

				++field;

			}

			if (onType == tz::KDayType_Dom)
			{
				return BasicDateTime<>(cp[0], cp[1], cp[2], tp[0], tp[1], tp[2], tp[3], tz::KTimeType_Wall);
			}
			else
			{
				//Last sunday == first sunday before 1st next month
				// Month
				cp[1] += 1;

				if (cp[1] > 12)
				{
					cp[1] = 1;
					// Year
					cp[0] += 1;
				}

				return BasicDateTime<>(cp[0], cp[1], 1, tp[0], tp[1], tp[2], tp[3], smalltime::RelSpec::SunBefore, tz::KTimeType_Wall);
			}

		}

		//======================================================================
		// Convert string to zone rule offset if possible, return if not
		//======================================================================
		RD Generator::ConvertToZoneRuleOffset(std::string str)
		{
			// Rule None
			if (str.empty() || str.find_first_not_of(' \t\n') == std::string::npos)
				return 0.0;

			//if(str.find_first_of('-') != std::string::npos)
			if (str == "-")
				return 0.0;

			// Rule Offset
			if (str.find_first_of(":") != std::string::npos)
				return ConvertTimeStrToRd(str);
			else
				return 0.0;
		}

		//======================================================================
		// Convert string to zone rule id if possible, return if not
		//======================================================================
		uint32_t Generator::ConvertToZoneRuleId(std::string str)
		{
			// Rule None
			if (str.empty() || str.find_first_not_of(' \t\n') == std::string::npos)
				return 0;

			//if(str.find_first_of('-') != std::string::npos)
			if (str == "-")
				return 0;

			// Rule Offset
			if (str.find_first_of(":") != std::string::npos)
				return 0;
			else
				return math::getUniqueID(str);
		}

		//=================================================
		// Convert month string to integer
		//==================================================
		int Generator::ConvertToMonth(const std::string& str)
		{
			if (str == "Jan")
				return 1;
			else if (str == "Feb")
				return 2;
			else if (str == "Mar")
				return 3;
			else if (str == "Apr")
				return 4;
			else if (str == "May")
				return 5;
			else if (str == "Jun")
				return 6;
			else if (str == "Jul")
				return 7;
			else if (str == "Aug")
				return 8;
			else if (str == "Sep")
				return 9;
			else if (str == "Oct")
				return 10;
			else if (str == "Nov")
				return 11;
			else if (str == "Dec")
				return 12;
			else
				return 0;
		}

		//================================================================
		// Check time type suffix from time string
		//=================================================================
		tz::TimeType Generator::CheckTimeSuffix(const std::string& tmStr)
		{
			char lastChar = tmStr.back();
			tz::TimeType tmType;

			switch (lastChar)
			{
			case 's':
				tmType = tz::KTimeType_Std;
				break;
			case 'w':
				tmType = tz::KTimeType_Wall;
				break;
			case 'u':
				tmType = tz::KTimeType_Utc;
				break;
			case 'z':
				tmType = tz::KTimeType_Utc;
				break;
			case 'g':
				tmType = tz::KTimeType_Utc;
				break;
			default:
				tmType = tz::KTimeType_Wall;
				break;
			}

			return tmType;

		}

		//==========================================================================
		// Check if day of month or some other specifier e.g. lastsun
		//==========================================================================
		tz::DayType Generator::CheckDayType(const std::string& str)
		{
			if (str.find("lastSun") != std::string::npos)
				return tz::KDayType_LastSun;

			if (str.find("Sun>=") != std::string::npos)
				return tz::KDayType_SunGE;

			return tz::KDayType_Dom;

		}

	}
}