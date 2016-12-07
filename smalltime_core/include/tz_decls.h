#pragma once
#ifndef _TZDECLS_
#define _TZDECLS_

#include "core_decls.h"
#include "time_math.h"
#include <cinttypes>

namespace smalltime
{
	namespace tz
	{
		static const int ONLY = -999;
		static const RD DMAX = 3651695.0; // (9999/1/1)
		static const int MAX = 9999;
		
		enum TimeType
		{
			KTimeType_Wall = 0x1,
			KTimeType_Std = 0x2,
			KTimeType_Utc = 0x4
		};

		enum DayType
		{
			KDayType_SunGE = 0,
			KDayType_LastSun = 1,
			KDayType_Dom = 2
		};

		enum RuleType
		{
			KRuleType_Offset = 0,
			KRuleType_None = 1,
			KRuleType_ID = 2
		};

		struct Rule
		{
			uint32_t rule_id;
			int from_year;
			int to_year;
			int month;
			int day;
			DayType day_type;
			RD at_time;
			TimeType at_type;
			RD offset;
			uint32_t letter;
		};

		struct Zone
		{
			uint32_t zone_id;
			uint32_t rule_id;
			RD mb_until_utc;
			TimeType until_type;
			RD zone_offset;
			RD next_zone_offset;
			RD mb_rule_offset;
			RD trans_rule_offset;
			uint64_t abbrev;
		};

		struct Link
		{
			uint32_t ref_zone;
			uint32_t target_zone;
		};

		struct Zones
		{
			uint32_t zone_id;
			int first;
			int size;
		};

		struct Rules
		{
			uint32_t rule_id;
			int first;
			int size;
		};

		class ZoneTransition
		{
		public:
			ZoneTransition(RD mb_trans_utc, RD cur_zoffset, RD next_zoffset, RD cur_roffset, RD next_roffset) :
				mb_trans_utc_(mb_trans_utc), cur_zoffset_(cur_zoffset), next_zoffset_(next_zoffset), cur_roffset_(cur_roffset),
				next_roffset_(next_roffset), mb_trans_wall_(mb_trans_utc_ + cur_zoffset_ + cur_roffset_), mb_trans_std_(mb_trans_utc_ + cur_zoffset_),
				trans_wall_(mb_trans_wall_ + math::MSEC()), trans_std_(mb_trans_std_ + math::MSEC()), trans_utc_(mb_trans_utc_ + math::MSEC()),
				first_inst_std_(trans_utc_ + next_zoffset_), first_inst_wall_(first_inst_std_ + next_roffset_)
			{

			}

			void Reset(RD mb_trans_utc, RD cur_zoffset, RD next_zoffset, RD cur_roffset, RD next_roffset)
			{
				cur_zoffset_ = cur_zoffset;
				next_zoffset_ = next_zoffset;
				cur_roffset_ = cur_roffset;
				next_roffset_ = next_roffset;

				mb_trans_utc_ = mb_trans_utc;
				mb_trans_std_ = mb_trans_utc_ + cur_zoffset_;
				mb_trans_wall_ = mb_trans_std_ + cur_roffset_;

				trans_utc_ = mb_trans_utc_ + math::MSEC();
				trans_std_ = trans_utc_ + cur_zoffset_;
				trans_wall_ = trans_std_ + cur_roffset_;

				first_inst_std_ = trans_utc_ + next_zoffset_;
				first_inst_wall_ = first_inst_std_ + next_roffset_;

			}

			RD mb_trans_wall_;
			RD mb_trans_std_;
			RD mb_trans_utc_;

			RD trans_wall_;
			RD trans_std_;
			RD trans_utc_;

			RD first_inst_wall_;
			RD first_inst_std_;

			RD cur_zoffset_;
			RD cur_roffset_;
			RD next_zoffset_;
			RD next_roffset_;

		};

		class RuleTransition
		{
		public:
			RuleTransition(RD mb_trans_utc, RD zoffset, RD cur_roffset, RD prev_roffset) :
				mb_trans_utc_(mb_trans_utc), zoffset_(zoffset), cur_roffset_(cur_roffset),
				prev_roffset_(prev_roffset), mb_trans_std_(mb_trans_utc_ + zoffset_), mb_trans_wall_(mb_trans_std_ + prev_roffset_),
				trans_utc_(mb_trans_utc_ + math::MSEC()), trans_std_(mb_trans_std_ + math::MSEC()), trans_wall_(mb_trans_wall_ + math::MSEC()),
				first_inst_std_(trans_utc_ + zoffset), first_inst_wall_(first_inst_std_ + cur_roffset_)
			{

			}

			void Reset(RD mb_trans_utc, RD zoffset, RD cur_roffset, RD prev_roffset)
			{
				zoffset_ = zoffset;
				cur_roffset_ = cur_roffset;
				prev_roffset_ = prev_roffset;

				mb_trans_utc_ = mb_trans_utc;
				mb_trans_std_ = mb_trans_utc_ + zoffset_;
				mb_trans_wall_ = mb_trans_std_ + prev_roffset_;

				trans_utc_ = mb_trans_utc_ + math::MSEC();
				trans_std_ = trans_utc_ + zoffset_;
				trans_wall_ = trans_std_ + prev_roffset_;

				first_inst_std_ = trans_utc_ + zoffset_;
				first_inst_wall_ = first_inst_std_ + cur_roffset_;
			}

			RD mb_trans_wall_;
			RD mb_trans_std_;
			RD mb_trans_utc_;

			RD trans_wall_;
			RD trans_std_;
			RD trans_utc_;

			RD first_inst_wall_;
			RD first_inst_std_;

			RD zoffset_;
			RD cur_roffset_;
			RD prev_roffset_;

		};

	}
}

#endif
