#include "Parser.h"
#include <sstream>

namespace smalltime
{
	namespace comp
	{
		
		//============================================================================
		// return container of all rules in file
		//===========================================================================
		bool Parser::parseRules(std::vector<RuleData>& vRuleData, std::ifstream& src)
		{
			std::string lineStr;
			src.clear();
			src.seekg(0, std::ios::beg);

			while (std::getline(src, lineStr))
			{
				//iterate line by line through file, searching for rules
				extractRule(vRuleData, lineStr);
			}

			return true;
		}

		//===============================================================================
		// return container of all zones in file
		//===================================================================================
		bool Parser::parseZones(std::vector<ZoneData>& vZoneData, std::ifstream& src)
		{
			std::string lineStr;
			src.clear();
			src.seekg(0, std::ios::beg);

			while (std::getline(src, lineStr))
			{
				//iterate line by line through file, searching for rules
				extractZone(vZoneData, lineStr);
			}

			return true;

		}

		//===============================================================================
		// return container of all zones in file
		//===================================================================================
		bool Parser::parseLinks(std::vector<LinkData>& vLinkData, std::ifstream& src)
		{
			std::string lineStr;
			src.clear();
			src.seekg(0, std::ios::beg);

			while (std::getline(src, lineStr))
			{
				//iterate line by line through file, searching for rules
				extractLink(vLinkData, lineStr);
			}

			return true;

		}

		//========================================================
		// check if line contains rule and add if it does
		//===========================================================
		bool Parser::extractRule(std::vector<RuleData>& vRuleData, const std::string& lineStr)
		{
			if (getLineType(lineStr) != LineType::Rule)
				return false;

			std::stringstream lineStream(lineStr);

			RuleData ir;
			// extract label - we can jsut discard it
			lineStream >> ir.name;
			//extract name
			lineStream >> ir.name;
			// extract from
			lineStream >> ir.from;
			// extract to
			lineStream >> ir.to;
			// extract type
			std::string type; //discard as unnecessary
			lineStream >> type;
			// extract in
			lineStream >> ir.in;
			// extract on
			lineStream >> ir.on;
			// extract at
			lineStream >> ir.at;
			// extract save
			lineStream >> ir.save;
			// extract letters
			lineStream >> ir.letters;

			vRuleData.push_back(ir);
			return true;

		}

		//========================================================
		// check if line contains zone and add if it does
		//===========================================================
		bool Parser::extractZone(std::vector<ZoneData>& vZoneData, const std::string& lineStr)
		{
			static std::string currZoneName = "";


			LineType lt = getLineType(lineStr);
			if (lt != LineType::ZoneHead && lt != LineType::ZoneBody)
				return false;

			std::string fmtLineStr = formatZoneLine(lineStr);
			std::stringstream lineStream(fmtLineStr);
			std::string fw;

			// extract the zone name from head line
			if (lt == LineType::ZoneHead)
			{
				//discard label
				lineStream >> fw;
				//get zone name
				lineStream >> fw;
				currZoneName = fw;

			}

			// zone name removed we can extract zone data
			ZoneData iz;
			iz.name = currZoneName;
			// extract gmt offset
			lineStream >> iz.gmtOffset;
			// extract rules
			lineStream >> iz.rule;
			// extract format
			lineStream >> iz.format;
			// extract remaining line since "until" is a variable length
			std::getline(lineStream, iz.until);

			vZoneData.push_back(iz);
			return true;

		}

		//============================================================
		// extract link data
		//============================================================
		bool Parser::extractLink(std::vector<LinkData>& vLinkData, const std::string& lineStr)
		{
			if (getLineType(lineStr) != LineType::Link)
				return false;

			std::stringstream lineStream(lineStr);

			LinkData il;
			// extract label 
			lineStream >> il.targetZoneName;
			// extract link 
			lineStream >> il.targetZoneName;
			lineStream >> il.refZoneName;

			vLinkData.push_back(il);
			return true;

		}

		//================================================
		// Determine the type of line
		//=================================================
		Parser::LineType Parser::getLineType(const std::string& lineStr)
		{
			std::string fw;
			std::stringstream lineStream(lineStr);
			// check first text to see line type
			lineStream >> fw;

			//check if comment line
			//lines are commented by single start # or whole line delimeter ################
			if (fw == "#" || fw.empty() || std::all_of(fw.begin(), fw.end(), [](char c) { if (c == '#') { return true; } else { return false; } }) == true)
				return LineType::Comment;
			//check if Rule Line
			else if (fw == "Rule")
				return LineType::Rule;
			// check if Link line
			else if (fw == "Link")
				return LineType::Link;
			// check if start of Zone 
			else if (fw == "Zone")
				return LineType::ZoneHead;
			else
				return LineType::ZoneBody;


		}

		//========================================================
		// removes any beginning whitespace or ending comments
		//========================================================
		std::string Parser::formatZoneLine(const std::string lineStr)
		{
			static const std::string whiteSpaceStr = " \t";

			// remove whitespace padding
			const auto strBegin = lineStr.find_first_not_of(whiteSpaceStr);
			if (strBegin == std::string::npos)
				return "";

			const auto strEnd = lineStr.find_last_not_of(whiteSpaceStr);
			const auto strRange = strEnd - strBegin + 1;

			std::string formattedStr = lineStr.substr(strBegin, strRange);

			// remove ending comments
			bool cmtFound = false;
			formattedStr.erase(std::remove_if(formattedStr.begin(), formattedStr.end(), [&](char c) {
				if (c == '#') { cmtFound = true; return true; }
				else if (cmtFound == true) { return true; }
				else { return false; }
			}), formattedStr.end());

			return formattedStr;
		}
	}
}