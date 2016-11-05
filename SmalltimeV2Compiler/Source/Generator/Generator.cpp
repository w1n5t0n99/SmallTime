#include "Generator.h"
#include <BasicDateTime.h>
#include <MathUtils.h>
#include <TimeMath.h>
#include <sstream>
#include <array>

namespace smalltime
{
	namespace comp
	{
		//============================================
		// Add a utc zone entry to list of zones
		//============================================
		bool Generator::proccessUtc(std::vector<tz::Zone>& vZone)
		{
			// Set a UTC entry
			tz::Zone utcZone = { math::getUniqueID("UTC") , 0, 0.0, tz::DMAX, tz::TimeType_Wall, 0.0, math::pack8Chars("UTC") };
			vZone.push_back(utcZone);

			return true;
		}

		//====================================================================================
		// Convert rule tokens to intermediate data, returns amount of rules processed
		//=======================================================================================
		bool Generator::processRules(std::vector<tz::Rule>& vRule, const std::vector<RuleData>& vRuleData)
		{
			//iterate through all rule tokens and convert
			for (const auto& ruleData : vRuleData)
			{
				tz::Rule rule;
				rule.ruleId = math::getUniqueID(ruleData.name);
				rule.fromYear = atoi(ruleData.from.c_str());

				//check if thier is  a "TO" year
				if (ruleData.to == "only")
					rule.toYear = rule.fromYear;
				else if (ruleData.to == "max")
					rule.toYear = tz::MAX;
				else
					rule.toYear = atoi(ruleData.to.c_str());

				// Month
				rule.month = convertToMonth(ruleData.in);

				//check if "ON" is day of month or not
				rule.dayType = checkDayType(ruleData.on);
				//rule.onType = onType;
				if (rule.dayType == tz::DayType_Dom)
					rule.day = atoi(ruleData.on.c_str());
				else if (rule.dayType == tz::DayType_SunGE)
					rule.day = atoi(ruleData.on.substr(5, ruleData.on.size() - 1).c_str());
				else if (rule.dayType == tz::DayType_LastSun)
					rule.day = 0;
				else
					rule.fromYear = 0;

				//check if "AT" is wall clock or something else
				rule.atType = checkTimeSuffix(ruleData.at);
				rule.atTime = convertTimeStrToRd(ruleData.at);

				rule.offset = convertTimeStrToRd(ruleData.save);
				rule.letter = math::pack4Chars(ruleData.letters);

				vRule.push_back(rule);
			}

			return true;

		}

		//======================================================================
		// Convert zone tokens into intermediate data
		//======================================================================
		bool Generator::processZones(std::vector<tz::Zone>& vZone, const std::vector<ZoneData>& vZoneData)
		{
			for (const auto& zl : vZoneData)
			{
				tz::Zone fzl;
				fzl.zoneId = math::getUniqueID(zl.name);
				//	fzl.rule = HashString(zl.rule);   //TODO: Rule can also be null "-" or a time offser e.g. 2:00

				fzl.ruleId = convertToZoneRuleId(zl.rule);
				fzl.ruleOffset = convertToZoneRuleOffset(zl.rule);

				fzl.zoneOffset = convertTimeStrToRd(zl.gmtOffset);
				fzl.abbrev = math::pack8Chars(zl.format);

				auto untilDt = convertZoneUntil(zl.until);
				fzl.until = untilDt.getRd();
				fzl.untilType = untilDt.getType();

				vZone.push_back(fzl);

			}

			return true;
		}

		//======================================================================
		// Convert link tokens into intermediate data
		//======================================================================
		bool Generator::processLinks(std::vector<tz::Link>& vLink, const std::vector<LinkData>& vLinkData)
		{
			for (const auto& ll : vLinkData)
			{
				tz::Link fll;

				fll.refZone = math::getUniqueID(ll.refZoneName);
				fll.targetZone = math::getUniqueID(ll.targetZoneName);
				vLink.push_back(fll);

			}

			return true;
		}

		//============================================================
		// Convert date/time string into integer data
		//==============================================================
		std::array<int, 4> Generator::convertTimeStrToFields(std::string str)
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
		RD Generator::convertTimeStrToRd(std::string tmStr)
		{
			auto tp = convertTimeStrToFields(tmStr);

			return math::rdFromTime(tp[0], tp[1], tp[2], tp[3]);
		}

		//==========================================================================
		// Convert "Until" zone data to a LocalDateTime to obtain FixedPoint value
		//==========================================================================
		BasicDateTime<> Generator::convertZoneUntil(std::string str)
		{
			// if string is empty or only contains whitespace
			// characters then must be a continuation line
			if (str.empty() || str.find_first_not_of(' \t\n') == std::string::npos)
				return BasicDateTime<>(tz::DMAX, tz::TimeType_Wall);

			std::stringstream sstr(str);
			std::string tempStr;

			int field = 0;
			std::array<int, 3> cp = { 1, 1, 1 };
			std::array<int, 4> tp = { 0, 0, 0, 0 };
			tz::TimeType timeType = tz::TimeType_Wall;
			tz::DayType onType = tz::DayType_Dom;

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
					cp[1] = convertToMonth(tempStr);
					break;
				case 2:
					//day - zones dont use Sun>=
					if (tempStr == "lastSun")
						onType = tz::DayType_LastSun;

					cp[2] = atoi(tempStr.c_str());
					break;
				case 3:
					//time
					timeType = checkTimeSuffix(tempStr);
					tp = convertTimeStrToFields(tempStr);   //If hour is 24, it should loop to 0 but need to error check!!
					break;
				}

				++field;

			}

			if (onType == tz::DayType_Dom)
			{
				return BasicDateTime<>(cp[0], cp[1], cp[2], tp[0], tp[1], tp[2], tp[3], tz::TimeType_Wall);
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

				return BasicDateTime<>(cp[0], cp[1], 1, tp[0], tp[1], tp[2], tp[3], smalltime::RelSpec::SunBefore, tz::TimeType_Wall);
			}

		}

		//======================================================================
		// Convert string to zone rule offset if possible, return if not
		//======================================================================
		RD Generator::convertToZoneRuleOffset(std::string str)
		{
			// Rule None
			if (str.empty() || str.find_first_not_of(' \t\n') == std::string::npos)
				return 0.0;

			//if(str.find_first_of('-') != std::string::npos)
			if (str == "-")
				return 0.0;

			// Rule Offset
			if (str.find_first_of(":") != std::string::npos)
				return convertTimeStrToRd(str);
			else
				return 0.0;
		}

		//======================================================================
		// Convert string to zone rule id if possible, return if not
		//======================================================================
		uint32_t Generator::convertToZoneRuleId(std::string str)
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
		int Generator::convertToMonth(const std::string& str)
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
		tz::TimeType Generator::checkTimeSuffix(const std::string& tmStr)
		{
			char lastChar = tmStr.back();
			tz::TimeType tmType;

			switch (lastChar)
			{
			case 's':
				tmType = tz::TimeType_Std;
				break;
			case 'w':
				tmType = tz::TimeType_Wall;
				break;
			case 'u':
				tmType = tz::TimeType_Utc;
				break;
			case 'z':
				tmType = tz::TimeType_Utc;
				break;
			case 'g':
				tmType = tz::TimeType_Utc;
				break;
			default:
				tmType = tz::TimeType_Wall;
				break;
			}

			return tmType;

		}

		//==========================================================================
		// Check if day of month or some other specifier e.g. lastsun
		//==========================================================================
		tz::DayType Generator::checkDayType(const std::string& str)
		{
			if (str.find("lastSun") != std::string::npos)
				return tz::DayType_LastSun;

			if (str.find("Sun>=") != std::string::npos)
				return tz::DayType_SunGE;

			return tz::DayType_Dom;

		}

	}
}