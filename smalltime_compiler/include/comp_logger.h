#pragma once
#ifndef _COMP_LOGGER_
#define _COMP_LOGGER_

#include <vector>
#include <ostream>

#include <core_decls.h>
#include <tz_decls.h>
#include "comp_decls.h"

namespace smalltime
{
	namespace comp
	{
		class CompLogger
		{
		public:
			void LogAllZones(std::ostream& stream, std::vector<tz::Zone>& vec_zone, std::vector<ZoneData>& vec_zonedata);

		};
	}
}

#endif
