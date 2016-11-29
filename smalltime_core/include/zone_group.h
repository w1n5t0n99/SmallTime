#pragma once
#ifndef _ZONE_GROUP_
#define _ZONE_GROUP_

#include "core_decls.h"
#include "tz_decls.h"
#include "basic_datetime.h"
#include "itzdbconnector.h"
#include <memory>

namespace smalltime
{
	namespace tz
	{
		class ZoneGroup
		{
		public:
			ZoneGroup(Zones zones, std::shared_ptr<ITzdbConnector> tzdb_connector);

			const Zone* const FindActiveZone(BasicDateTime<> cur_dt, Choose choose);

			const Zone* const FindPreviousZone(int cur_zone_index);
			const Zone* const FindNextZone(int cur_zone_index);

			const Zone* const CorrectForAmbigWallOrUtc(const BasicDateTime<>& cur_dt, int cur_zone_index, Choose choose);

		private:
			const Zone* const zone_arr_;
			const Zones zones_;
			std::shared_ptr<ITzdbConnector> tzdb_connector_;

		};
	}
}

#endif 
