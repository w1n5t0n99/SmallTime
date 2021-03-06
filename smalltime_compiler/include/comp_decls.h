#pragma once
#ifndef _COMPDECLS_
#define _COMPDECLS_

#include <core_decls.h>
#include <string>
#include <vector>

namespace smalltime
{
	namespace comp
	{
		enum Transition
		{
			KTransition_MomentBeforeUtc = 0,
			KTransition_MomentBeforeWall = 1,
			KTransition_TransitionWall = 2,
			KTransition_FirstInstWall = 3
		};

		struct RuleData
		{
			std::string name;
			std::string from;
			std::string to;
			//  std::string type;  //unused 
			std::string in;
			std::string on;
			std::string at;
			std::string save;
			std::string letters;

		};

		struct ZoneData
		{
			std::string name;
			std::string gmt_offset;
			std::string rule;
			std::string format;
			std::string until;
		};

		struct LinkData
		{
			std::string ref_zone_name;
			std::string target_zone_name;
		};

		struct MetaData
		{
			RD max_zone_offset;
			RD min_zone_offset;
			RD max_rule_offset;
			int max_zone_size;
			int max_rule_size;
		};

	}
}

#endif
