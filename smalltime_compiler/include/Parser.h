#pragma once
#ifndef _PARSER_
#define _PARSER_

#include <core_decls.h>
#include "../comp_decls.h"

#include <fstream>

namespace smalltime
{
	namespace comp
	{
		class Parser
		{
		public:
			bool ParseRules(std::vector<RuleData>& vec_ruledata, std::ifstream& src);
			bool ParseZones(std::vector<ZoneData>& vec_zonedata, std::ifstream& src);
			bool ParseLinks(std::vector<LinkData>& vec_linkdata, std::ifstream& src);

		private:
			enum class LineType : char
			{
				KRule,
				KZoneHead,
				KZoneBody,
				KLink,
				KComment
			};

			bool ExtractRule(std::vector<RuleData>& vec_ruledata, const std::string& line_str);
			bool ExtractZone(std::vector<ZoneData>& vec_zonedata, const std::string& line_str);
			bool ExtractLink(std::vector<LinkData>& vec_linkdata, const std::string& line_str);

			LineType GetLineType(const std::string& line_str);
			std::string FormatZoneLine(const std::string line_str);

		};
	}
}

#endif