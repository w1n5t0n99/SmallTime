#pragma once
#ifndef _ZONE_POST_GENERATOR_
#define _ZONE_POST_GENERATOR_

#include <memory>

#include <core_decls.h>
#include <tz_decls.h>
#include <tzdb_connector_interface.h>
#include "comp_decls.h"

namespace smalltime
{
	namespace comp
	{
		class ZonePostGenerator
		{
		public:
			ZonePostGenerator(std::shared_ptr<tz::TzdbConnectorInterface> tzdb_connector);

			bool ProcessZones();

		private:
			std::pair<RD, RD> GetWallTransition(int cur_zone_index, tz::Zone const * const zone_arr, int zone_arr_size);
			std::pair<RD, RD> GetUtcTransition(int cur_zone_index, tz::Zone const * const zone_arr, int zone_arr_size);

			tz::Zone const * const GetNextZoneInGroup(int cur_zone_index, tz::Zone const * const zone_arr, int zone_arr_size);

			std::shared_ptr<tz::TzdbConnectorInterface> tzdb_connector_;

		};

	}
}

#endif
