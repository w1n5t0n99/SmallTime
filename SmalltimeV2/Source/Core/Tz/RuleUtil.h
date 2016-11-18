#pragma once
#ifndef _RULEUTIL_
#define _RULEUTIL_

#include <CoreDecls.h>
#include <TzDecls.h>
#include <BasicDateTime.h>

namespace smalltime
{
	namespace tz
	{
		class RuleUtil
		{
		public:
			RuleUtil(uint32_t rule_id, const Zone* const zone);

			const Rule* const FindActiveRule(BasicDateTime<> cur_dt);
			const Rule* const FindPreviousRule(BasicDateTime<> cur_rule);
			int FindClosestActiveYear(int year);

			void InitTransitionData(int year);
			void BuildTransitionData(RD* buffer, int year);

			BasicDateTime<> CalcTransitionFull(const Rule* const rule, int year, TimeType time_type);
			BasicDateTime<> CalcTransitionWall(const Rule* const rule, int year);
			BasicDateTime<> CalcTransitionStd(const Rule* const rule, int year);
			BasicDateTime<> CalcTransitionUtc(const Rule* const rule, int year);
			BasicDateTime<> CalcTransitionFast(const Rule* const rule, int year);
		
		private:
			const Rule* rule_arr_;
			const Rules rules_;
			const Zone* const zone_;
			int current_year_;
			int primary_pool_year_;
			int secondary_pool_year_;

		};

	}
}

#endif
