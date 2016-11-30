#include "../include/parser.h"
#include <sstream>

namespace smalltime
{
	namespace comp
	{
		
		//============================================================================
		// return container of all rules in file
		//===========================================================================
		bool Parser::ParseRules(std::vector<RuleData>& vec_ruledata, std::ifstream& src)
		{
			std::string line_str;
			src.clear();
			src.seekg(0, std::ios::beg);

			while (std::getline(src, line_str))
			{
				//iterate line by line through file, searching for rules
				ExtractRule(vec_ruledata, line_str);
			}

			return true;
		}

		//===============================================================================
		// return container of all zones in file
		//===================================================================================
		bool Parser::ParseZones(std::vector<ZoneData>& vec_zonedata, std::ifstream& src)
		{
			std::string line_str;
			src.clear();
			src.seekg(0, std::ios::beg);

			while (std::getline(src, line_str))
			{
				//iterate line by line through file, searching for rules
				ExtractZone(vec_zonedata, line_str);
			}

			return true;

		}

		//===============================================================================
		// return container of all zones in file
		//===================================================================================
		bool Parser::ParseLinks(std::vector<LinkData>& vec_linkdata, std::ifstream& src)
		{
			std::string line_str;
			src.clear();
			src.seekg(0, std::ios::beg);

			while (std::getline(src, line_str))
			{
				//iterate line by line through file, searching for rules
				ExtractLink(vec_linkdata, line_str);
			}

			return true;

		}

		//========================================================
		// check if line contains rule and add if it does
		//===========================================================
		bool Parser::ExtractRule(std::vector<RuleData>& vec_ruledata, const std::string& line_str)
		{
			if (GetLineType(line_str) != LineType::KRule)
				return false;

			std::stringstream lineStream(line_str);

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

			vec_ruledata.push_back(ir);
			return true;

		}

		//========================================================
		// check if line contains zone and add if it does
		//===========================================================
		bool Parser::ExtractZone(std::vector<ZoneData>& vec_zonedata, const std::string& line_str)
		{
			static std::string cur_zone_name = "";


			LineType lt = GetLineType(line_str);
			if (lt != LineType::KZoneHead && lt != LineType::KZoneBody)
				return false;

			std::string fmt_line_str = FormatZoneLine(line_str);
			std::stringstream line_stream(fmt_line_str);
			std::string fw;

			// extract the zone name from head line
			if (lt == LineType::KZoneHead)
			{
				//discard label
				line_stream >> fw;
				//get zone name
				line_stream >> fw;
				cur_zone_name = fw;

			}

			// zone name removed we can extract zone data
			ZoneData iz;
			iz.name = cur_zone_name;
			// extract gmt offset
			line_stream >> iz.gmt_offset;
			// extract rules
			line_stream >> iz.rule;
			// extract format
			line_stream >> iz.format;
			// extract remaining line since "until" is a variable length
			std::getline(line_stream, iz.until);

			vec_zonedata.push_back(iz);
			return true;

		}

		//============================================================
		// extract link data
		//============================================================
		bool Parser::ExtractLink(std::vector<LinkData>& vec_linkdata, const std::string& line_str)
		{
			if (GetLineType(line_str) != LineType::KLink)
				return false;

			std::stringstream line_stream(line_str);

			LinkData il;
			// extract label 
			line_stream >> il.target_zone_name;
			// extract link 
			line_stream >> il.target_zone_name;
			line_stream >> il.ref_zone_name;

			vec_linkdata.push_back(il);
			return true;

		}

		//================================================
		// Determine the type of line
		//=================================================
		Parser::LineType Parser::GetLineType(const std::string& line_str)
		{
			std::string fw;
			std::stringstream line_stream(line_str);
			// check first text to see line type
			line_stream >> fw;

			//check if comment line
			//lines are commented by single start # or whole line delimeter ################
			if (fw == "#" || fw.empty() || std::all_of(fw.begin(), fw.end(), [](char c) { if (c == '#') { return true; } else { return false; } }) == true)
				return LineType::KComment;
			//check if Rule Line
			else if (fw == "Rule")
				return LineType::KRule;
			// check if Link line
			else if (fw == "Link")
				return LineType::KLink;
			// check if start of Zone 
			else if (fw == "Zone")
				return LineType::KZoneHead;
			else
				return LineType::KZoneBody;


		}

		//========================================================
		// removes any beginning whitespace or ending comments
		//========================================================
		std::string Parser::FormatZoneLine(const std::string line_str)
		{
			static const std::string white_space = " \t";

			// remove whitespace padding
			const auto begin_str = line_str.find_first_not_of(white_space);
			if (begin_str == std::string::npos)
				return "";

			const auto end_str = line_str.find_last_not_of(white_space);
			const auto range_str = end_str - begin_str + 1;

			std::string formatted_str = line_str.substr(begin_str, range_str);

			// remove ending comments
			bool cmtFound = false;
			formatted_str.erase(std::remove_if(formatted_str.begin(), formatted_str.end(), [&](char c) {
				if (c == '#') { cmtFound = true; return true; }
				else if (cmtFound == true) { return true; }
				else { return false; }
			}), formatted_str.end());

			return formatted_str;
		}
	}
}