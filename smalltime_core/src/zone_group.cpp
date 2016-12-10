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
		// Find the active time zone 
		//==============================================
		const Zone* const ZoneGroup::FindActiveZone(BasicDateTime<> cur_dt, Choose choose)
		{
			const Zone* cur_zone = nullptr;
			int closest_zone_index = 0;
			int last_zone_index = zones_.first + zones_.size - 1;
			ZoneTransition zt(0.0, 0.0, 0.0, 0.0, 0.0);

			for (int i = zones_.first; i <= last_zone_index; ++i)
			{
				zt.Reset(zone_arr_[i].mb_until_utc, zone_arr_[i].zone_offset, zone_arr_[i].next_zone_offset, zone_arr_[i].mb_rule_offset, zone_arr_[i].trans_rule_offset);

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

			return CorrectForAmbigAny(cur_dt, closest_zone_index, zt, choose);

		}

		//===========================================================
		// Find the active and previous zone
		//=========================================================
		std::pair<const Zone* const, const Zone* const> ZoneGroup::FindActiveAndPreviousZone(BasicDateTime<> cur_dt, Choose choose)
		{
			const Zone* cur_zone = nullptr;
			const Zone* prev_zone = nullptr;
			int closest_zone_index = -1;
			int prev_zone_index = -1;
			int last_zone_index = zones_.first + zones_.size - 1;
			ZoneTransition zt(0.0, 0.0, 0.0, 0.0, 0.0);

			for (int i = zones_.first; i <= last_zone_index; ++i)
			{
				zt.Reset(zone_arr_[i].mb_until_utc, zone_arr_[i].zone_offset, zone_arr_[i].next_zone_offset, zone_arr_[i].mb_rule_offset, zone_arr_[i].trans_rule_offset);

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
					closest_zone_index = i;
					cur_zone = &zone_arr_[closest_zone_index];
					
					if (i > 0)
					{
						prev_zone_index = i - 1;
						prev_zone = &zone_arr_[prev_zone_index];
					}
					break;
				}

			}


			if (cur_zone == nullptr)
				return std::make_pair(prev_zone, cur_zone);

			return CorrectPairForAmbigAny(cur_dt, closest_zone_index, zt, choose);
		}

		//========================================================
		// Correct for ambigousness between zones
		//========================================================
		const Zone* const ZoneGroup::CorrectForAmbigAny(const BasicDateTime<>& cur_dt, int cur_zone_index, const ZoneTransition& cur_zone_transition, Choose choose)
		{

			auto cur_zone = &zone_arr_[cur_zone_index];
			auto next_zone = FindNextZone(cur_zone_index);

			RD mb_any = 0.0;
			RD fi_any = 0.0;
			TimeType time_type = KTimeType_Wall;

			switch (cur_dt.GetType())
			{
			case KTimeType_Wall:
				mb_any = cur_zone_transition.mb_trans_wall_;
				fi_any = cur_zone_transition.first_inst_wall_;
				time_type = KTimeType_Wall;
				break;
			case KTimeType_Std:
				mb_any = cur_zone_transition.mb_trans_std_;
				fi_any = cur_zone_transition.first_inst_std_;
				time_type = KTimeType_Std;
				break;
			case KTimeType_Utc:
				mb_any = cur_zone_transition.mb_trans_utc_;
				fi_any = cur_zone_transition.trans_utc_;
				time_type = KTimeType_Utc;
				break;
			}

			// check for ambig with current and next zone
			if ((fi_any <= cur_dt.GetFixed() || AlmostEqualUlps(fi_any, cur_dt.GetFixed(), 11)) &&
				(cur_dt.GetFixed() <= mb_any || AlmostEqualUlps(mb_any, cur_dt.GetFixed(), 11)))
			{
				switch (choose)
				{
				// Ambigiuous local time gap
				case Choose::KEarliest:
					return cur_zone;
				case Choose::KLatest:
					return next_zone;
				case Choose::KError:
					throw TimeZoneAmbigMultiException(BasicDateTime<>(fi_any, time_type), BasicDateTime<>(mb_any, time_type));
				}
			}

			// check for ambig with previous zone and current zone
			auto prev_zone = FindPreviousZone(cur_zone_index);
			if (!prev_zone)
				return cur_zone;

			ZoneTransition prev_zone_transition(prev_zone->mb_until_utc, prev_zone->zone_offset, prev_zone->next_zone_offset, prev_zone->mb_rule_offset, prev_zone->trans_rule_offset);

			mb_any = 0.0;
			fi_any = 0.0;
			time_type = KTimeType_Wall;

			switch (cur_dt.GetType())
			{
			case KTimeType_Wall:
				mb_any = prev_zone_transition.mb_trans_wall_;
				fi_any = prev_zone_transition.first_inst_wall_;
				time_type = KTimeType_Wall;
				break;
			case KTimeType_Std:
				mb_any = prev_zone_transition.mb_trans_std_;
				fi_any = prev_zone_transition.first_inst_std_;
				time_type = KTimeType_Std;
				break;
			case KTimeType_Utc:
				mb_any = prev_zone_transition.mb_trans_utc_;
				fi_any = prev_zone_transition.trans_utc_;
				time_type = KTimeType_Utc;
				break;
			}

			if (mb_any < cur_dt.GetFixed() && cur_dt.GetFixed() < fi_any)
			{
				switch (choose)
				{
				// Ambigiuous local time gap
				case Choose::KEarliest:
					return prev_zone;
				case Choose::KLatest:
					return cur_zone;
				case Choose::KError:
					throw TimeZoneAmbigNoneException(BasicDateTime<>(mb_any, time_type), BasicDateTime<>(fi_any, time_type));
				}
			}

			return cur_zone;


		}

		//========================================================
		// Correct for ambigousness between zones
		//========================================================
		std::pair<const Zone* const, const Zone* const> ZoneGroup::CorrectPairForAmbigAny(const BasicDateTime<>& cur_dt, int cur_zone_index, const ZoneTransition& cur_zone_transition, Choose choose)
		{

			auto cur_zone = &zone_arr_[cur_zone_index];
			auto prev_zone = FindPreviousZone(cur_zone_index);
			auto next_zone = FindNextZone(cur_zone_index);

			RD mb_any = 0.0;
			RD fi_any = 0.0;
			TimeType time_type = KTimeType_Wall;

			switch (cur_dt.GetType())
			{
			case KTimeType_Wall:
				mb_any = cur_zone_transition.mb_trans_wall_;
				fi_any = cur_zone_transition.first_inst_wall_;
				time_type = KTimeType_Wall;
				break;
			case KTimeType_Std:
				mb_any = cur_zone_transition.mb_trans_std_;
				fi_any = cur_zone_transition.first_inst_std_;
				time_type = KTimeType_Std;
				break;
			case KTimeType_Utc:
				mb_any = cur_zone_transition.mb_trans_utc_;
				fi_any = cur_zone_transition.trans_utc_;
				time_type = KTimeType_Utc;
				break;
			}

			// check for ambig with current and next zone
			if ((fi_any <= cur_dt.GetFixed() || AlmostEqualUlps(fi_any, cur_dt.GetFixed(), 11)) &&
				(cur_dt.GetFixed() <= mb_any || AlmostEqualUlps(mb_any, cur_dt.GetFixed(), 11)))
			{
				switch (choose)
				{
					// Ambigiuous local time gap
				case Choose::KEarliest:
					return std::make_pair(prev_zone, cur_zone);
				case Choose::KLatest:
					return std::make_pair(cur_zone, next_zone);
				case Choose::KError:
					throw TimeZoneAmbigMultiException(BasicDateTime<>(fi_any, time_type), BasicDateTime<>(mb_any, time_type));
				}
			}

			// check for ambig with previous zone and current zone
			if (!prev_zone)
				return std::make_pair(nullptr, cur_zone);

			auto prev_prev_zone = FindPreviousZone(cur_zone_index - 1);

			ZoneTransition prev_zone_transition(prev_zone->mb_until_utc, prev_zone->zone_offset, prev_zone->next_zone_offset, prev_zone->mb_rule_offset, prev_zone->trans_rule_offset);

			mb_any = 0.0;
			fi_any = 0.0;
			time_type = KTimeType_Wall;

			switch (cur_dt.GetType())
			{
			case KTimeType_Wall:
				mb_any = prev_zone_transition.mb_trans_wall_;
				fi_any = prev_zone_transition.first_inst_wall_;
				time_type = KTimeType_Wall;
				break;
			case KTimeType_Std:
				mb_any = prev_zone_transition.mb_trans_std_;
				fi_any = prev_zone_transition.first_inst_std_;
				time_type = KTimeType_Std;
				break;
			case KTimeType_Utc:
				mb_any = prev_zone_transition.mb_trans_utc_;
				fi_any = prev_zone_transition.trans_utc_;
				time_type = KTimeType_Utc;
				break;
			}

			if (mb_any < cur_dt.GetFixed() && cur_dt.GetFixed() < fi_any)
			{
				switch (choose)
				{
					// Ambigiuous local time gap
				case Choose::KEarliest:
					return std::make_pair(prev_prev_zone, prev_zone);
				case Choose::KLatest:
					return std::make_pair(prev_zone, cur_zone);
				case Choose::KError:
					throw TimeZoneAmbigNoneException(BasicDateTime<>(mb_any, time_type), BasicDateTime<>(fi_any, time_type));
				}
			}

			return std::make_pair(prev_zone, cur_zone);

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