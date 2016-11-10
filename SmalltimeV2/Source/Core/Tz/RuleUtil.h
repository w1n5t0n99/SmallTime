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
			const Zone* m_zone;

		};

	}
}

#endif
