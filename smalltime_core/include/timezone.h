#pragma once
#ifndef _TIMEZONE_
#define _TIMEZONE_

#include <string>
#include <memory>

#include "core_decls.h"
#include "tzdb_connector_interface.h"

namespace smalltime
{
	namespace tz
	{
		class TimeZone
		{
		public:

			RD FixedOffsetFromLocal(RD rd, std::string time_zone_name, Choose choose, std::shared_ptr<TzdbConnectorInterface> tzdb_connector);
			RD FixedOffsetFromUtc(RD rd, std::string time_zone_name, std::shared_ptr<TzdbConnectorInterface> tzdb_connector);

		};
	}
}

#endif
