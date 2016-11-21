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

			const Rule* const FindActiveRule(BasicDateTime<> cur_dt, Choose choose);

			std::pair<const Rule* const, int> FindPreviousRule(BasicDateTime<> cur_rule);
			std::pair<const Rule* const, int> FindNextRule(BasicDateTime<> cur_rule);
			const Rule* const FindCorrectedForAmbigRule(BasicDateTime<> cur_dt, BasicDateTime<> cur_rule_dt, Choose choose);

			int FindClosestActiveYear(int year);
			int FindPreviousActiveYear(int year);
			int FindNextActiveYear(int year);

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
			int current_year_, primary_year_, previous_year_, next_year_;
			RD* primary_ptr_, *previous_ptr_, *next_ptr_;
		};

	}
}

#endif
