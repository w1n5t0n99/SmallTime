#include "../include/zone_post_generator.h"

#include <basic_datetime.h>
#include <rule_group.h>
#include <time_math.h>

#include <iostream>

namespace smalltime
{
	namespace comp
	{

		//===========================================
		// Ctor
		//===========================================
		ZonePostGenerator::ZonePostGenerator(std::shared_ptr<tz::TzdbConnectorInterface> tzdb_connector) : tzdb_connector_(tzdb_connector)
		{

		}

		//=====================================================
		// Process zones - add transition data
		//=====================================================
		bool ZonePostGenerator::ProcessZones(std::vector<tz::Zone>& vec_zone)
		{
			// util_wall stores temp until 
			//until_utc stores temp rule offset
			auto zone_arr_size = vec_zone.size();

			for (int i = 0; i < zone_arr_size; ++i)
			{
				auto zt = CalcZoneData(i, vec_zone);

				vec_zone[i].next_zone_offset = zt.next_zoffset_;
				vec_zone[i].mb_until_utc = zt.mb_trans_utc_;
				vec_zone[i].mb_rule_offset = zt.cur_roffset_;
				vec_zone[i].trans_rule_offset = zt.next_roffset_;
			}

			return true;
		}

	

		//====================================================
		// Calculate zone transition data
		//====================================================
		tz::ZoneTransition ZonePostGenerator::CalcZoneData(int cur_zone_index, std::vector<tz::Zone>& vec_zone)
		{
			//ZoneDate - last_moment_utc, last_moment_wall, transition_wall, first_inst_wall

			const auto& cur_zone = vec_zone[cur_zone_index];

			RD next_zoffset = 0.0;
			RD next_roffset = 0.0;
			auto cur_zoffset = cur_zone.zone_offset;
			auto cur_roffset = cur_zone.mb_rule_offset;

			BasicDateTime<> until_dt(cur_zone.mb_until_utc, cur_zone.until_type);
			//the cur zone hasnt been altered yet so until_wall still stores zone basic transition
			BasicDateTime<> mb_until_dt(cur_zone.mb_until_utc - math::MSEC(), cur_zone.until_type);
			// get current rule offset at moment before transition if any
			if (cur_zone.rule_id > 0)
			{
				// find the rule offset if active
				tz::RuleGroup rg(cur_zone.rule_id, &cur_zone, tzdb_connector_);
				auto rule = rg.FindActiveRuleNoCheck(mb_until_dt);
				if (rule != nullptr)
					cur_roffset += rule->offset;
			}

			auto next_zone_index = GetNextZoneInGroup(cur_zone_index, vec_zone);
			// the last zone is ongoing
			if (next_zone_index != -1)
			{
				const auto& next_zone = vec_zone[next_zone_index];

				// find the current offset and the next offset
				//the next zone hasnt been altered yet so until_utc still stores zone save
				next_zoffset = next_zone.zone_offset;
				next_roffset = next_zone.mb_rule_offset;

				// get next rule offset if any
				if (next_zone.rule_id > 0)
				{
					// find the rule offset if active
					tz::RuleGroup rg(next_zone.rule_id, &next_zone, tzdb_connector_);
					auto rule = rg.FindActiveRuleNoCheck(until_dt);
					if (rule != nullptr)
						next_roffset += rule->offset;
				}
			}
			else
			{
				next_zoffset = cur_zoffset;
				next_roffset = cur_roffset;
			}

			RD mb_trans_utc = 0.0;
			switch (mb_until_dt.GetType())
			{
			case tz::KTimeType_Wall:
				mb_trans_utc = mb_until_dt.GetFixed() - cur_roffset - cur_zoffset;
				break;
			case tz::KTimeType_Std:
				mb_trans_utc = mb_until_dt.GetFixed() - cur_zoffset;
				break;
			case tz::KTimeType_Utc:
				mb_trans_utc = mb_until_dt.GetFixed();
				break;
			}

			return tz::ZoneTransition(mb_trans_utc, cur_zoffset, next_zoffset, cur_roffset, next_roffset);
		}

		//=================================================
		// Find next zone of same group in any
		//=================================================
		int ZonePostGenerator::GetNextZoneInGroup(int cur_zone_index, std::vector<tz::Zone>& vec_zone)
		{
			if ((cur_zone_index + 1) < vec_zone.size())
			{
				auto cur_zone = vec_zone[cur_zone_index];
				auto next_zone = vec_zone[cur_zone_index + 1];

				if (cur_zone.zone_id == next_zone.zone_id)
					return cur_zone_index + 1;
				else
					return -1;
			}
			else
			{
				return -1;
			}
		}

	}
}