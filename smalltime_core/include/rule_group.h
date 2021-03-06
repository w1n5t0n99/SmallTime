#pragma once
#ifndef _RULEGROUP_
#define _RULEGROUP_

#include "core_decls.h"
#include "tz_decls.h"
#include "basic_datetime.h"
#include "tzdb_connector_interface.h"
#include <memory>
#include <vector>

namespace smalltime
{
	namespace tz
	{
		class RuleGroup
		{
		public:
			RuleGroup(Rules rules, const Rule* const rule_arr, const Zone* const zone, const Zone* const prev_zone);

			const Rule* const FindActiveRule(BasicDateTime<> cur_dt, Choose choose);
			const Rule* const FindActiveRuleNoCheck(BasicDateTime<> cur_dt);

		private:
			std::pair<const Rule* const, int> FindPreviousRule(BasicDateTime<> cur_rule);
			std::pair<const Rule* const, int> FindPreviousRule(RD cur_rule);

			std::pair<const Rule* const, int> FindNextRule(BasicDateTime<> cur_rule);
			std::pair<const Rule* const, int> FindNextRule(RD cur_rule);

			const Rule* const CorrectForAmbigAny(const BasicDateTime<>& cur_dt, RuleTransition cur_rule_transition, const Rule* const cur_rule, Choose choose);

			int FindClosestActiveYear(int year);
			int FindPreviousActiveYear(int year);
			int FindNextActiveYear(int year);

			void InitTransitionData(int year);
			void BuildTransitionData(std::vector<std::pair<RD, int> >& transition_vec, int year);


			BasicDateTime<> CalcTransitionFast(const Rule* const rule, int year);
			RuleTransition CalcRuleData(const Rule* const rule, int year);
		
		private:
			const Rule* const rule_arr_;
			const Rules rules_;

			const Zone* const zone_;
			const Zone* const prev_zone_;
			const ZoneTransition zone_transition_;
			const ZoneTransition prev_zone_transition_;

			int current_year_, primary_year_, previous_year_, next_year_;

			std::vector<std::pair<RD, int> > primary_year_transitions_;
			std::vector<std::pair<RD, int> > prev_year_transitions_;
			std::vector<std::pair<RD, int> > next_year_transitions_;

			// There's usually, at most two active rules per year
			static const int KSTART_SIZE = 3;
		};

	}
}

#endif
