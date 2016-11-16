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

			const Rule* FindPreviousRule(const Rule* const rule, int year);
			int FindClosestActiveYear(const int* const buffer, int year);

			void InitTransitionYear(int year);
			void BuildTransitionData(RD* buffer, int year);
			void BuildClosestYearData(int* buffer, int year);

			BasicDateTime<> CalcTransitionWall(const Rule* const rule, int year);
			BasicDateTime<> CalcTransitionStd(const Rule* const rule, int year);
			BasicDateTime<> CalcTransitionUtc(const Rule* const rule, int year);
			BasicDateTime<> CalcTransitionFast(const Rule* const rule, int year);

			//=========================================================
			RuleUtil(RuleGroup group, const Zone* zone) : m_group(group), m_zone(zone) {}

			BasicDateTime<> calcWallTransition(const Rule* rule, int year);

			const Rule* findPreviousRule(const Rule* rule, int year);

		private:
			RD calcFastTransition(const Rule* rule, int transitionYear);
			RD calcFastWithTimeTransition(const Rule* rule, int transitionYear);

			void setFastTransitions(int transitionYear);
			int setClosestActiveYears(int year);

			const Rule* calcClosestRuleBefore(RD rd);

			RuleGroup m_group;
			const Zone* const m_zone;
			//====================================================

			const Rule* rule_arr_;
			const Rules rules_;
			const Zone* const zone_;
			int current_year_;

		};

	}
}

#endif
