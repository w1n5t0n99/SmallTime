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
				auto wall_transition = GetWallTransition(i, vec_zone);
				auto utc_transition = GetUtcTransition(i, vec_zone);

				auto transition_data = CalcZoneData(i, vec_zone);
				auto& mb_rd = std::get<KTransitionType_MomentBeforeWall>(transition_data);
				auto& fi_rd = std::get<KTransitionType_FirstInstWall>(transition_data);

				BasicDateTime<> mb_dt(mb_rd, tz::KTimeType_Wall);
				BasicDateTime<> fi_dt(fi_rd, tz::KTimeType_Wall);

				auto& cr = mb_dt;
				printf("Zone %d - MB => %d/%d/%d %d:%d:%d:%d  FI =>",vec_zone[i].zone_id, cr.GetYear(), cr.GetMonth(), cr.GetDay(), cr.GetHour(), cr.GetMinute(), cr.GetSecond(), cr.GetMillisecond());
				printf(" %d/%d/%d %d:%d:%d:%d\n", fi_dt.GetYear(), fi_dt.GetMonth(), fi_dt.GetDay(), fi_dt.GetHour(), fi_dt.GetMinute(), fi_dt.GetSecond(), fi_dt.GetMillisecond());



				vec_zone[i].until_wall = wall_transition.first;
				vec_zone[i].until_utc = utc_transition.first;
				vec_zone[i].until_diff = wall_transition.second;
			}

			return true;
		}

		//====================================================
		// Calculate zone transition in wall time
		//====================================================
		std::pair<RD, RD> ZonePostGenerator::GetWallTransition(int cur_zone_index, std::vector<tz::Zone>& vec_zone)
		{
			const auto& cur_zone = vec_zone[cur_zone_index];
			auto next_zone_index = GetNextZoneInGroup(cur_zone_index, vec_zone);

			// the last zone is ongoing
			if (next_zone_index < 0)
				return std::make_pair(tz::DMAX, 0.0);

			const auto& next_zone = vec_zone[next_zone_index];

			// Calc offset at transition
			//---------------------------------------------------------------------
			//the cur zone hasnt been altered yet so until_wall still stores zone basic transition
			BasicDateTime<> until_dt(cur_zone.until_wall, cur_zone.until_type);
			//the next zone hasnt been altered yet so until_utc still stores zone save
			auto until_zone_offset = next_zone.zone_offset;
			auto until_rule_offset = next_zone.until_utc;

			// get next rule offset if any
			if (next_zone.rule_id > 0)
			{
				// find the rule offset if active
				tz::RuleGroup rg(next_zone.rule_id, &next_zone, tzdb_connector_);
				auto rule = rg.FindActiveRuleNoCheck(until_dt);
				if (rule != nullptr)
					until_rule_offset += rule->offset;
			}

			auto total_until_offset = until_zone_offset + until_rule_offset;

			// Calc offset moment before transition
			//-------------------------------------------------------------
			auto cur_zone_offset = cur_zone.zone_offset;
			auto cur_rule_offset = cur_zone.until_utc;
			// find the rule offset if active
			BasicDateTime<> cur_dt(cur_zone.until_wall - math::MSEC(), cur_zone.until_type);
			tz::RuleGroup rg(cur_zone.rule_id, &cur_zone, tzdb_connector_);
			auto rule = rg.FindActiveRuleNoCheck(cur_dt);
			if (rule != nullptr)
				cur_rule_offset += rule->offset;

			auto total_cur_offset = cur_zone_offset + cur_rule_offset;
	
			// Convert transition to correct time type
			//--------------------------------------------------------------
			std::pair<RD, RD> ret_val = std::make_pair(0.0, 0.0);
			if (cur_zone.until_type == tz::KTimeType_Wall)
			{
				ret_val = std::make_pair(cur_zone.until_wall, total_until_offset -  total_cur_offset);
			}
			else if (cur_zone.until_type == tz::KTimeType_Std)
			{
				ret_val = std::make_pair(cur_zone.until_wall + until_rule_offset, total_until_offset - total_cur_offset);
			}
			else
			{
				ret_val = std::make_pair(cur_zone.until_wall + until_zone_offset + until_rule_offset, total_until_offset - total_cur_offset);
			}

			return ret_val;

		}

		//====================================================
		// Calculate zone transition data
		//====================================================
		std::tuple<RD, RD, RD, RD> ZonePostGenerator::CalcZoneData(int cur_zone_index, std::vector<tz::Zone>& vec_zone)
		{
			//ZoneDate - last_moment_utc, last_moment_wall, transition_wall, first_inst_wall

			const auto& cur_zone = vec_zone[cur_zone_index];
			auto next_zone_index = GetNextZoneInGroup(cur_zone_index, vec_zone);

			// the last zone is ongoing
			if (next_zone_index < 0)
				return std::make_tuple(tz::DMAX, tz::DMAX, tz::DMAX, tz::DMAX);

			const auto& next_zone = vec_zone[next_zone_index];

			//the cur zone hasnt been altered yet so until_wall still stores zone basic transition
			BasicDateTime<> until_dt(cur_zone.until_wall, cur_zone.until_type);
			// find the current offset and the next offset
			//the next zone hasnt been altered yet so until_utc still stores zone save
			auto next_zone_offset = next_zone.zone_offset;
			auto next_rule_offset = next_zone.until_utc;
		
			// get next rule offset if any
			if (next_zone.rule_id > 0)
			{
				// find the rule offset if active
				tz::RuleGroup rg(next_zone.rule_id, &next_zone, tzdb_connector_);
				auto rule = rg.FindActiveRuleNoCheck(until_dt);
				if (rule != nullptr)
					next_rule_offset += rule->offset;
			}

			auto cur_zone_offset = cur_zone.zone_offset;
			auto cur_rule_offset = cur_zone.until_utc;
			//the cur zone hasnt been altered yet so until_wall still stores zone basic transition
			BasicDateTime<> mb_until_dt(cur_zone.until_wall - math::MSEC(), cur_zone.until_type);
			// get current rule offset at moment before transition if any
			if (cur_zone.rule_id > 0)
			{
				// find the rule offset if active
				tz::RuleGroup rg(cur_zone.rule_id, &cur_zone, tzdb_connector_);
				auto rule = rg.FindActiveRuleNoCheck(mb_until_dt);
				if (rule != nullptr)
					cur_rule_offset += rule->offset;
			}

			auto moment_before_utc_dt = GetUtcTime(mb_until_dt, cur_zone_offset, cur_rule_offset);
			auto moment_before_wall_dt = GetWallTime(mb_until_dt, cur_zone_offset, cur_rule_offset);
			auto transition_wall_dt = GetWallTime(until_dt, next_zone_offset, next_rule_offset);
			auto first_inst_wall_dt = GetWallTime(BasicDateTime<>(moment_before_utc_dt.GetFixed() + math::MSEC(), tz::KTimeType_Utc), next_zone_offset, next_rule_offset);

			return std::make_tuple(moment_before_utc_dt.GetFixed(),
				moment_before_wall_dt.GetFixed(),
				transition_wall_dt.GetFixed(),
				first_inst_wall_dt.GetFixed());
		}

		//=======================================================
		// Convert given datetime to utc time
		//=======================================================
		BasicDateTime<> ZonePostGenerator::GetUtcTime(BasicDateTime<> dt, RD zone_offset, RD rule_offset)
		{
			if (dt.GetType() == tz::KTimeType_Utc)
				return dt;
			else if (dt.GetType() == tz::KTimeType_Std)
				return BasicDateTime<>(dt.GetFixed() - zone_offset, tz::KTimeType_Utc);
			else
				return BasicDateTime<>(dt.GetFixed() - zone_offset - rule_offset, tz::KTimeType_Utc);
		}

		//=======================================================
		// Convert given datetime to utc time
		//=======================================================
		BasicDateTime<> ZonePostGenerator::GetWallTime(BasicDateTime<> dt, RD zone_offset, RD rule_offset)
		{
			if (dt.GetType() == tz::KTimeType_Wall)
				return dt;
			else if (dt.GetType() == tz::KTimeType_Std)
				return BasicDateTime<>(dt.GetFixed() + rule_offset, tz::KTimeType_Wall);
			else
				return BasicDateTime<>(dt.GetFixed() + zone_offset + rule_offset, tz::KTimeType_Wall);
		}

		//====================================================
		// Calculate zone transition in utc time
		//====================================================
		std::pair<RD, RD> ZonePostGenerator::GetUtcTransition(int cur_zone_index, std::vector<tz::Zone>& vec_zone)
		{
			const auto& cur_zone = vec_zone[cur_zone_index];
			auto next_zone_index = GetNextZoneInGroup(cur_zone_index, vec_zone);

			// the last zone is ongoing
			if (next_zone_index < 0)
				return std::make_pair(tz::DMAX, 0.0);
			
			const auto& next_zone = vec_zone[next_zone_index];

			// Calc offset at transition
			//---------------------------------------------------------------------
			//the cur zone hasnt been altered yet so until_wall still stores zone basic transition
			BasicDateTime<> until_dt(cur_zone.until_wall, cur_zone.until_type);
			//the next zone hasnt been altered yet so until_utc still stores zone save
			auto until_zone_offset = next_zone.zone_offset;
			auto until_rule_offset = next_zone.until_utc;

			// get next rule offset if any
			if (next_zone.rule_id > 0)
			{
				// find the rule offset if active
				tz::RuleGroup rg(next_zone.rule_id, &next_zone, tzdb_connector_);
				auto rule = rg.FindActiveRuleNoCheck(until_dt);
				if (rule != nullptr)
					until_rule_offset += rule->offset;
			}

			// Calc offset moment before transition
			//-------------------------------------------------------------
			auto total_until_offset = until_zone_offset + until_rule_offset;
			// calculate the offset diff
			auto cur_zone_offset = cur_zone.zone_offset;
			auto cur_rule_offset = cur_zone.until_utc;
			// find the rule offset if active
			BasicDateTime<> cur_dt(cur_zone.until_wall - math::MSEC(), cur_zone.until_type);
			tz::RuleGroup rg(cur_zone.rule_id, &cur_zone, tzdb_connector_);
			auto rule = rg.FindActiveRuleNoCheck(cur_dt);
			if (rule != nullptr)
				cur_rule_offset += rule->offset;

			auto total_cur_offset = cur_zone_offset + cur_rule_offset;

			// Convert transition to correct time type
			//--------------------------------------------------------------
			std::pair<RD, RD> ret_val = std::make_pair(0.0, 0.0);
			if (cur_zone.until_type == tz::KTimeType_Utc)
			{
				ret_val = std::make_pair(cur_zone.until_wall, total_until_offset - total_cur_offset);
			}
			else if (cur_zone.until_type == tz::KTimeType_Std)
			{
				ret_val = std::make_pair(cur_zone.until_wall - until_zone_offset, total_until_offset - total_cur_offset);
			}
			else
			{
				ret_val = std::make_pair(cur_zone.until_wall - until_zone_offset - until_rule_offset, total_until_offset - total_cur_offset);
			}

			return ret_val;

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