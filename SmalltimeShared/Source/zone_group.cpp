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
		ZoneGroup::ZoneGroup(Zones zones, std::shared_ptr<ITzdbConnector> tzdb_connector) : tzdb_connector_(tzdb_connector),  zones_(zones),
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

			if (cur_dt.GetType() == KTimeType_Wall)
			{
				for (int i = zones_.first; i < zones_.first + zones_.size; ++i)
				{
					cur_zone_index = i;
					if (cur_dt.GetFixed() < zone_arr_[i].until_wall || AlmostEqualUlps(cur_dt.GetFixed(), zone_arr_[i].until_wall, 11))
						return &zone_arr_[i];
				}
			}
			else if (cur_dt.GetType() == KTimeType_Utc)
			{
				for (int i = zones_.first; i < zones_.first + zones_.size; ++i)
				{
					cur_zone_index = i;
					if (cur_dt.GetFixed() < zone_arr_[i].until_utc || AlmostEqualUlps(cur_dt.GetFixed(), zone_arr_[i].until_utc, 11))
						return &zone_arr_[i];
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
			auto prev_zone = FindPreviousZone(cur_zone_index);

			if (prev_zone)
			{
				RD offset_diff = 0.0;
				RD cur_zone_rd = 0.0;
				if (cur_dt.GetType() == KTimeType_Wall)
				{
					cur_zone_rd = cur_zone->until_wall;
					offset_diff = cur_zone_rd - prev_zone->until_wall;
				}
				else if (cur_dt.GetType() == KTimeType_Utc)
				{
					cur_zone_rd = cur_zone->until_utc;
					offset_diff = cur_zone_rd - prev_zone->until_utc;
				}

				RD cur_zone_inst = 0.0;
				if (cur_dt.GetType() == KTimeType_Wall)
					cur_zone_inst = cur_zone_rd + offset_diff;
				else if (cur_dt.GetType() == KTimeType_Utc)
					cur_zone_inst = cur_zone_rd - offset_diff;

				if ((cur_dt.GetFixed() >= cur_zone_rd || AlmostEqualUlps(cur_dt.GetFixed(), cur_zone_rd, 11)) && cur_dt.GetFixed() < cur_zone_inst)
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
							throw TimeZoneAmbigNoneException(BasicDateTime<>(cur_zone_rd, KTimeType_Wall), BasicDateTime<>(cur_zone_inst - math::MSEC(), KTimeType_Wall));
						else if (cur_dt.GetType() == KTimeType_Utc)
							throw TimeZoneAmbigMultiException(BasicDateTime<>(cur_zone_rd, KTimeType_Utc), BasicDateTime<>(cur_zone_inst - math::MSEC(), KTimeType_Utc));
					}
				}
			}
			// check for ambiguousness with next active rule
			auto next_zone = FindNextZone(cur_zone_index);
			if (next_zone)
			{
				RD offset_diff = 0.0;
				RD next_zone_rd = 0.0;
				if (cur_dt.GetType() == KTimeType_Wall)
				{
					next_zone_rd = next_zone->until_wall;
					offset_diff = next_zone_rd - cur_zone->until_wall;
					RD next_zone_inst = next_zone_rd + offset_diff;

					if ((cur_dt.GetFixed() >= next_zone_inst || AlmostEqualUlps(cur_dt.GetFixed(), next_zone_inst, 11)) && cur_dt.GetFixed() < next_zone_rd)
					{
						// Ambiguous multiple local times
						if (choose == Choose::KEarliest)
							return cur_zone;
						else if (choose == Choose::KLatest)
							return next_zone;
						else
							throw TimeZoneAmbigMultiException(BasicDateTime<>(next_zone_inst, KTimeType_Wall), BasicDateTime<>(next_zone_rd - math::MSEC(), KTimeType_Wall));
					}
				}
			}
				
			return nullptr;
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