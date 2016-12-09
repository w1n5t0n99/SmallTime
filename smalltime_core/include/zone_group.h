#pragma once
#ifndef _ZONE_GROUP_
#define _ZONE_GROUP_

#include "core_decls.h"
#include "tz_decls.h"
#include "basic_datetime.h"
#include "tzdb_connector_interface.h"
#include <memory>

namespace smalltime
{
	namespace tz
	{
		class ZoneGroup
		{
		public:
			ZoneGroup(Zones zones, std::shared_ptr<TzdbConnectorInterface> tzdb_connector);

			const Zone* const FindActiveZone(BasicDateTime<> cur_dt, Choose choose);

		private:
			const Zone* const FindPreviousZone(int cur_zone_index);
			const Zone* const FindNextZone(int cur_zone_index);

			const Zone* const CorrectForAmbigAny(const BasicDateTime<>& cur_dt, int cur_zone_index, const ZoneTransition& cur_zone_transition, Choose choose);

		private:
			const Zone* const zone_arr_;
			const Zones zones_;
			std::shared_ptr<TzdbConnectorInterface> tzdb_connector_;
		};
	}
}

#endif 
