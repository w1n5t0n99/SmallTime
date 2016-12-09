#pragma once
#ifndef _RULEGROUP_
#define _RULEGROUP_

#include "core_decls.h"
#include "tz_decls.h"
#include "basic_datetime.h"
#include "tzdb_connector_interface.h"
#include <memory>

namespace smalltime
{
	namespace tz
	{
		class RuleGroup
		{
		public:
			RuleGroup(uint32_t rule_id, const Zone* const zone, std::shared_ptr<TzdbConnectorInterface> tzdb_connector);

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
			void BuildTransitionData(RD* buffer, int year);

			BasicDateTime<> CalcTransitionFast(const Rule* const rule, int year);
			RuleTransition CalcRuleData(const Rule* const rule, int year);
		
		private:
			const Rule* rule_arr_;
			const Rules rules_;

			const Zone* const zone_;
			const ZoneTransition zone_transition_;

			int current_year_, primary_year_, previous_year_, next_year_;
			RD* primary_ptr_, *previous_ptr_, *next_ptr_;
			std::shared_ptr<TzdbConnectorInterface> tzdb_connector_;
		};

	}
}

#endif
