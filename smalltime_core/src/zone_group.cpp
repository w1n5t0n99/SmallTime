#include "../include/zone_group.h"

#include <iostream>
#include <assert.h>
#include <array>

#include "../include/smalltime_exceptions.h"
#include "../include/float_util.h"
#include <time_math.h>
#include <smalltime_exceptions.h>

namespace smalltime
{
	namespace tz
	{
		//=============================================
		// Ctor
		//=============================================
		ZoneGroup::ZoneGroup(Zones zones, std::shared_ptr<TzdbConnectorInterface> tzdb_connector) : tzdb_connector_(tzdb_connector),  zones_(zones),
			zone_arr_(tzdb_connector->GetZoneHandle())
		{

		}

		//===========================================
		// Find the active time zone if any
		//==============================================
		const Zone* const ZoneGroup::FindActiveZone(BasicDateTime<> cur_dt, Choose choose)
		{
			const Zone* cur_zone = nullptr;
			int cur_zone_index = 0;
			int last_zone_index = zones_.first + zones_.size - 1;

			if (cur_dt.GetType() == KTimeType_Wall)
			{
				for (int i = zones_.first; i <= last_zone_index; ++i)
				{
					cur_zone_index = i;

					if (cur_dt.GetFixed() < zone_arr_[i].until_wall || cur_zone_index == last_zone_index)
					{
						cur_zone = &zone_arr_[i];
						break;
					}
				}
			}
			else if (cur_dt.GetType() == KTimeType_Utc)
			{
				for (int i = zones_.first; i <= last_zone_index; ++i)
				{
					cur_zone_index = i;
					if (cur_dt.GetFixed() < zone_arr_[i].until_utc || cur_zone_index == last_zone_index)
					{
						cur_zone = &zone_arr_[i];
						break;
					}
				}
			}

			if (cur_zone == nullptr)
				return cur_zone;

			return CorrectForAmbigWallOrUtc(cur_dt, cur_zone_index, choose);

		}

		//======================================================
		// Check if the cur dt is within an ambiguous range
		//======================================================
		const Zone* const ZoneGroup::CorrectForAmbigWallOrUtc(const BasicDateTime<>& cur_dt, int cur_zone_index, Choose choose)
		{
			auto cur_zone = &zone_arr_[cur_zone_index];
			auto next_zone = FindNextZone(cur_zone_index);

			auto next_zone_inst = 0.0;
			if(cur_dt.GetType() == KTimeType_Wall)
				next_zone_inst = cur_zone->until_wall + cur_zone->until_diff;
			else if(cur_dt.GetType() == KTimeType_Utc)
				next_zone_inst = cur_zone->until_wall - cur_zone->until_diff;

			if (cur_dt.GetFixed() > next_zone_inst || AlmostEqualUlps(cur_dt.GetFixed(), next_zone_inst, 11))
			{
				// Ambigiuous local time gap
				if (choose == Choose::KEarliest)
				{
					return cur_zone;
				}
				else if (choose == Choose::KLatest)
				{
					return next_zone;
				}
				else
				{
					if (cur_dt.GetType() == KTimeType_Wall)
						throw TimeZoneAmbigMultiException(BasicDateTime<>(cur_zone->until_wall, KTimeType_Wall), BasicDateTime<>(next_zone_inst, KTimeType_Wall));
					else if (cur_dt.GetType() == KTimeType_Utc)
						throw TimeZoneAmbigMultiException(BasicDateTime<>(cur_zone->until_utc, KTimeType_Utc), BasicDateTime<>(next_zone_inst, KTimeType_Utc));
				}
			}

			// check for ambiguousness with previous active zone
			auto prev_zone = FindPreviousZone(cur_zone_index);
			if (prev_zone)
			{
				auto prev_zone_inst = 0.0;
				if (cur_dt.GetType() == KTimeType_Wall)
					prev_zone_inst = prev_zone->until_wall + prev_zone->until_diff;
				else if (cur_dt.GetType() == KTimeType_Utc)
					prev_zone_inst = prev_zone->until_utc - prev_zone->until_diff;

				if (cur_dt.GetFixed() < prev_zone_inst)
				{
					// Ambigiuous local time gap
					if (choose == Choose::KEarliest)
					{
						return prev_zone;
					}
					else if (choose == Choose::KLatest)
					{
						return cur_zone;
					}
					else
					{
						if (cur_dt.GetType() == KTimeType_Wall)
							throw TimeZoneAmbigNoneException(BasicDateTime<>(prev_zone->until_wall, KTimeType_Wall), BasicDateTime<>(prev_zone_inst, KTimeType_Wall));
						else if (cur_dt.GetType() == KTimeType_Utc)
							throw TimeZoneAmbigMultiException(BasicDateTime<>(prev_zone->until_utc, KTimeType_Utc), BasicDateTime<>(prev_zone_inst, KTimeType_Utc));
					}
				}
			}
			
			return cur_zone;
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