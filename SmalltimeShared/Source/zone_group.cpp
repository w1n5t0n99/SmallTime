#include "zone_group.h"
#include "smalltime_exceptions.h"
#include "floatutil.h"
#include <TimeMath.h>
#include <iostream>
#include <assert.h>
#include <array>

namespace smalltime
{
	namespace tz
	{
		//=============================================
		// Ctor
		//=============================================
		ZoneGroup::ZoneGroup(uint32_t zone_id, std::shared_ptr<ITzdbConnector> tzdb_connector) : tzdb_connector_(tzdb_connector),  zones_(tzdb_connector_->FindZones(zone_id)),
			zone_arr_(tzdb_connector_->GetZoneHandle())
		{

		}

		//===========================================
		// Find the active time zone if any
		//==============================================
		const Zone* const ZoneGroup::FindActiveZone(BasicDateTime<> cur_dt, Choose choose)
		{
			if (cur_dt.getType() == KTimeType_Wall)
			{
				for (int i = zones_.first; i < zones_.first + zones_.size; ++i)
				{

				}
			}
		}

		//=============================================
		// Find previous zone if any
		//=============================================
		const Zone* const ZoneGroup::FindPreviousZone(int cur_zone_index)
		{
			if (cur_zone_index <= zones_.first)
				return nullptr;

			return &zone_arr_[cur_zone_index - 1];
		}

		//==============================================
		// Find next zone if any
		//==============================================
		const Zone* const ZoneGroup::FindNextZone(int cur_zone_index)
		{
			if (cur_zone_index >= (zones_.first + zones_.size - 1))
				return nullptr;

			return &zone_arr_[cur_zone_index + 1];

		}

	}
}