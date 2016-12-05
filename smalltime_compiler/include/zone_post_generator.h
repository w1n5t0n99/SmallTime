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

			bool ProcessZones(std::vector<tz::Zone>& vec_zone);

		private:
			std::pair<RD, RD> GetWallTransition(int cur_zone_index, std::vector<tz::Zone>& vec_zone);
			std::pair<RD, RD> GetUtcTransition(int cur_zone_index, std::vector<tz::Zone>& vec_zone);

			std::tuple<RD, RD, RD> CalcZoneData(int cur_zone_index, std::vector<tz::Zone>& vec_zone);
			RD GetUtcTime(BasicDateTime<> dt, RD zone_offset, RD rule_offset);
			RD GetWallTime(BasicDateTime<> dt, RD zone_offset, RD rule_offset);

			int GetNextZoneInGroup(int cur_zone_index, std::vector<tz::Zone>& vec_zone);

			std::shared_ptr<tz::TzdbConnectorInterface> tzdb_connector_;

		};

	}
}

#endif
