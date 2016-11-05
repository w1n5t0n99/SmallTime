#pragma once
#ifndef _PARSER_
#define _PARSER_

#include <CoreDecls.h>
#include "../CompDecls.h"

#include <fstream>

namespace smalltime
{
	namespace comp
	{
		class Parser
		{
		public:
			bool parseRules(std::vector<RuleData>& vRuleData, std::ifstream& src);
			bool parseZones(std::vector<ZoneData>& vZoneData, std::ifstream& src);
			bool parseLinks(std::vector<LinkData>& vLinkData, std::ifstream& src);

		private:
			enum class LineType : char
			{
				Rule,
				ZoneHead,
				ZoneBody,
				Link,
				Comment
			};

			bool extractRule(std::vector<RuleData>& vRuleData, const std::string& lineStr);
			bool extractZone(std::vector<ZoneData>& vZoneData, const std::string& lineStr);
			bool extractLink(std::vector<LinkData>& vLinkData, const std::string& lineStr);

			LineType getLineType(const std::string& lineStr);
			std::string formatZoneLine(const std::string lineStr);

		};
	}
}

#endif