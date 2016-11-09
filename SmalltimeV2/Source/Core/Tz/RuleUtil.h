#pragma once
#ifndef _RULEUTIL_
#define _RULEUTIL_

#include <CoreDecls.h>
#include <TzDecls.h>
#include <BasicDateTime.h>
#include <vector>


namespace smalltime
{
	namespace tz
	{
		class RuleUtil
		{
		public:
			RuleUtil(RuleGroup group, const Zone* zone) : m_group(group), m_zone(zone), m_prevRules() {};

			const Rule* findPreviousRule(const Rule* rule, int year);

		private:
			RD calculateFastTransition(const Rule* rule, int transitionYear);
			void calculateAllFastTransitions(int transitionYear);

			const Rule* findClosestRuleBefore(RD rd);
			int findClosestYearWithActiveRules(int year);

			RuleGroup m_group;
			const Zone* m_zone;
			std::vector<std::pair<const Rule*, RD> > m_prevRules;

		};
	}
}

#endif
