#pragma once
#ifndef _TIMEZONE_
#define _TIMEZONE_

#include <string>
#include <memory>

#include "core_decls.h"
#include "timezone_db.h"

namespace smalltime
{
	namespace tz
	{
		class TimeZone
		{
		public:

			RD FixedOffsetFromLocal(RD rd, std::string time_zone_name, Choose choose);
			RD FixedOffsetFromUtc(RD rd, std::string time_zone_name);

		private:
			static TimeZoneDB timezone_db_;
		};
	}
}

#endif
