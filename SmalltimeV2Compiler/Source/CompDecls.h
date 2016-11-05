#pragma once
#ifndef _COMPDECLS_
#define _COMPDECLS_

#include <string>
#include <vector>

namespace smalltime
{
	namespace comp
	{
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
			std::string gmtOffset;
			std::string rule;
			std::string format;
			std::string until;
		};

		struct LinkData
		{
			std::string refZoneName;
			std::string targetZoneName;
		};

	}
}

#endif
