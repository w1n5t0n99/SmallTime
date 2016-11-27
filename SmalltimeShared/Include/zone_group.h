#pragma once
#ifndef _ZONE_GROUP_
#define _ZONE_GROUP_

#include <CoreDecls.h>
#include <TzDecls.h>
#include <BasicDateTime.h>
#include <itzdbconnector.h>
#include <memory>

namespace smalltime
{
	namespace tz
	{
		class ZoneGroup
		{
		public:
			ZoneGroup(uint32_t zone_id, std::shared_ptr<ITzdbConnector> tzdb_connector);

			const Zone* const FindActiveZone(BasicDateTime<> cur_dt, Choose choose);

			const Zone* const FindPreviousZone(int cur_zone_index);
			const Zone* const FindNextZone(int cur_zone_index);

			const Rule* const CorrectForAmbigWallOrUtc(const BasicDateTime<>& cur_dt, const BasicDateTime<>& cur_zone_dt, const Zone* const cur_zone, Choose choose);

		private:
			const Zone* const zone_arr_;
			const Zones zones_;
			std::shared_ptr<ITzdbConnector> tzdb_connector_;

		};
	}
}

#endif 
