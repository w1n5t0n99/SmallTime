#include "../include/zone_group.h"

#include <iostream>
#include <assert.h>
#include <array>

#include "../include/smalltime_exceptions.h"
#include "../include/float_util.h"
#include <time_math.h>
#include <smalltime_exceptions.h>
#include <rule_group.h>

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
			int closest_zone_index = 0;
			int last_zone_index = zones_.first + zones_.size - 1;

			for (int i = zones_.first; i <= last_zone_index; ++i)
			{
				ZoneTransition zt(zone_arr_[i].mb_until_utc, zone_arr_[i].zone_offset, zone_arr_[i].next_zone_offset, zone_arr_[i].mb_rule_offset, zone_arr_[i].trans_rule_offset);

				RD trans_any = 0.0;
				switch (cur_dt.GetType())
				{
				case KTimeType_Wall:
					trans_any = zt.trans_wall_;
					break;
				case KTimeType_Std:
					trans_any = zt.trans_std_;
					break;
				case KTimeType_Utc:
					trans_any = zt.trans_utc_;
					break;
				}

				if (cur_dt.GetFixed() < trans_any || i == last_zone_index)
				{
					cur_zone = &zone_arr_[i];
					closest_zone_index = i;
					break;
				}

			}

		
			if (cur_zone == nullptr)
				return cur_zone;

			return CorrectForAmbigAny(cur_dt, closest_zone_index, choose);

		}

		//========================================================
		// Correct for ambigousness between zones
		//========================================================
		const Zone* const ZoneGroup::CorrectForAmbigAny(const BasicDateTime<>& cur_dt, int cur_zone_index, const ZoneTransition& cur_zone_transition, Choose choose)
		{
			/*
			if (cur_dt.GetType() == KTimeType_Wall)
				return CorrectForAmbigWall(cur_dt, cur_zone_index, choose);
			else if (cur_dt.GetType() == KTimeType_Std)
				return CorrectForAmbigStd(cur_dt, cur_zone_index, choose);
			else
				return CorrectForAmbigUtc(cur_dt, cur_zone_index, choose);
				*/

			auto cur_zone = &zone_arr_[cur_zone_index];
			auto next_zone = FindNextZone(cur_zone_index);

			RD cur_mb_any = 0.0;
			RD cur_fi_any = 0.0;

			switch (cur_dt.GetType())
			{
			case KTimeType_Wall:
				cur_mb_any = cur_zone_transition.mb_trans_wall_;
				cur_fi_any = cur_zone_transition.first_inst_wall_;
				break;
			case KTimeType_Std:
				cur_mb_any = cur_zone_transition.mb_trans_std_;
				cur_fi_any = cur_zone_transition.first_inst_std_;
				break;
			case KTimeType_Utc:
				cur_mb_any = cur_zone_transition.mb_trans_utc_;
				cur_fi_any = cur_zone_transition.trans_utc_;
				break;
			}

			// check for mu


		}

		//======================================================
		// Check if the cur dt is within an ambiguous range
		//======================================================
		const Zone* const ZoneGroup::CorrectForAmbigWall(const BasicDateTime<>& cur_dt, int cur_zone_index, Choose choose)
		{
			auto cur_zone = &zone_arr_[cur_zone_index];
			auto next_zone = FindNextZone(cur_zone_index);


			if ((cur_zone->trans_rule_offset <= cur_dt.GetFixed() || AlmostEqualUlps(cur_zone->trans_rule_offset, cur_dt.GetFixed(), 11)) && 
				(cur_dt.GetFixed() <= cur_zone->next_zone_offset || AlmostEqualUlps(cur_zone->next_zone_offset, cur_dt.GetFixed(), 11)))
			{
				// Ambigiuous local time gap
				if (choose == Choose::KEarliest)
					return cur_zone;
				else if (choose == Choose::KLatest)
					return next_zone;
				else
					throw TimeZoneAmbigMultiException(BasicDateTime<>(cur_zone->trans_rule_offset, KTimeType_Wall), BasicDateTime<>(cur_zone->next_zone_offset, KTimeType_Wall));
			}

			// check for ambiguousness with previous active zone
			auto prev_zone = FindPreviousZone(cur_zone_index);
			if (prev_zone)
			{
				if ((prev_zone->next_zone_offset <= cur_dt.GetFixed() || AlmostEqualUlps(prev_zone->trans_rule_offset, cur_dt.GetFixed(), 11)) &&
					cur_dt.GetFixed() < prev_zone->trans_rule_offset )
				{
					// Ambigiuous local time gap
					if (choose == Choose::KEarliest)
						return prev_zone;
					else if (choose == Choose::KLatest)
						return cur_zone;
					else
						throw TimeZoneAmbigNoneException(BasicDateTime<>(prev_zone->next_zone_offset, KTimeType_Wall), BasicDateTime<>(prev_zone->trans_rule_offset, KTimeType_Wall));
				}
			}
			
			return cur_zone;
		}

		//======================================================
		// Check if the cur dt is within an ambiguous range
		//======================================================
		const Zone* const ZoneGroup::CorrectForAmbigUtc(const BasicDateTime<>& cur_dt, int cur_zone_index, Choose choose)
		{
			auto cur_zone = &zone_arr_[cur_zone_index];
			auto next_zone = FindNextZone(cur_zone_index);

			// ambiguousness only occurs in wall time, so cur_dt must be converted to wall time to check
			RD zone_offset = cur_zone->zone_offset;
			RD rule_offset = 0.0;
			if (cur_zone->rule_id > 0)
			{
				RuleGroup rg(cur_zone->rule_id, cur_zone, tzdb_connector_);
				auto r = rg.FindActiveRule(cur_dt, Choose::KError);
				rule_offset += r->offset;
			}

			auto cur_wall_rd = cur_dt.GetFixed() + zone_offset + rule_offset;

			if ((cur_zone->trans_rule_offset <= cur_wall_rd || AlmostEqualUlps(cur_zone->trans_rule_offset, cur_wall_rd, 11)) &&
				(cur_wall_rd <= cur_zone->next_zone_offset || AlmostEqualUlps(cur_zone->next_zone_offset, cur_wall_rd, 11)))
			{
				// Ambigiuous local time gap
				if (choose == Choose::KEarliest)
					return cur_zone;
				else if (choose == Choose::KLatest)
					return next_zone;
				else
					throw TimeZoneAmbigMultiException(BasicDateTime<>(cur_zone->trans_rule_offset, KTimeType_Wall), BasicDateTime<>(cur_zone->next_zone_offset, KTimeType_Wall));
			}

			// check for ambiguousness with previous active zone
			auto prev_zone = FindPreviousZone(cur_zone_index);
			if (prev_zone)
			{
				if ((prev_zone->next_zone_offset <= cur_wall_rd || AlmostEqualUlps(prev_zone->trans_rule_offset, cur_wall_rd, 11)) &&
					cur_wall_rd < prev_zone->trans_rule_offset)
				{
					// Ambigiuous local time gap
					if (choose == Choose::KEarliest)
						return prev_zone;
					else if (choose == Choose::KLatest)
						return cur_zone;
					else
						throw TimeZoneAmbigNoneException(BasicDateTime<>(prev_zone->next_zone_offset, KTimeType_Wall), BasicDateTime<>(prev_zone->trans_rule_offset, KTimeType_Wall));
				}
			}

			return cur_zone;
		}

		//======================================================
		// Check if the cur dt is within an ambiguous range
		//======================================================
		const Zone* const ZoneGroup::CorrectForAmbigStd(const BasicDateTime<>& cur_dt, int cur_zone_index, Choose choose)
		{
			auto cur_zone = &zone_arr_[cur_zone_index];
			auto next_zone = FindNextZone(cur_zone_index);

			// ambiguousness only occurs in wall time, so cur_dt must be converted to wall time to check
			RD rule_offset = 0.0;
			if (cur_zone->rule_id > 0)
			{
				RuleGroup rg(cur_zone->rule_id, cur_zone, tzdb_connector_);
				auto r = rg.FindActiveRule(cur_dt, Choose::KError);
				rule_offset += r->offset;
			}

			auto cur_wall_rd = cur_dt.GetFixed() + rule_offset;

			if ((cur_zone->trans_rule_offset <= cur_wall_rd || AlmostEqualUlps(cur_zone->trans_rule_offset, cur_wall_rd, 11)) &&
				(cur_wall_rd <= cur_zone->next_zone_offset || AlmostEqualUlps(cur_zone->next_zone_offset, cur_wall_rd, 11)))
			{
				// Ambigiuous local time gap
				if (choose == Choose::KEarliest)
					return cur_zone;
				else if (choose == Choose::KLatest)
					return next_zone;
				else
					throw TimeZoneAmbigMultiException(BasicDateTime<>(cur_zone->trans_rule_offset, KTimeType_Wall), BasicDateTime<>(cur_zone->next_zone_offset, KTimeType_Wall));
			}

			// check for ambiguousness with previous active zone
			auto prev_zone = FindPreviousZone(cur_zone_index);
			if (prev_zone)
			{
				if ((prev_zone->next_zone_offset <= cur_wall_rd || AlmostEqualUlps(prev_zone->trans_rule_offset, cur_wall_rd, 11)) &&
					cur_wall_rd < prev_zone->trans_rule_offset)
				{
					// Ambigiuous local time gap
					if (choose == Choose::KEarliest)
						return prev_zone;
					else if (choose == Choose::KLatest)
						return cur_zone;
					else
						throw TimeZoneAmbigNoneException(BasicDateTime<>(prev_zone->next_zone_offset, KTimeType_Wall), BasicDateTime<>(prev_zone->trans_rule_offset, KTimeType_Wall));
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