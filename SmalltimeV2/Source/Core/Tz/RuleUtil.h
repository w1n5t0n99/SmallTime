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
			RuleUtil(RuleGroup group, const Zone* zone);

			const Rule* findPreviousRule(const Rule* rule, int year);

		private:
			RD calculateFastTransition(const Rule* rule, int transitionYear, bool withTime = false);
			void calculateAllFastTransitions(const Rule* rule, int transitionYear, bool withTime = false);

			const Rule* findClosestRuleBefore(RD rd);

			RuleGroup m_group;
			const Zone* zone;
			std::vector<std::pair<const Rule*, RD> > m_prevRules;

		};
	}
}

#endif
