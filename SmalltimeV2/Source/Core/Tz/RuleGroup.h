#pragma once
#ifndef _RULEGROUP_
#define _RULEGROUP_

#include <CoreDecls.h>
#include <TzDecls.h>
#include <BasicDateTime.h>

namespace smalltime
{
	namespace tz
	{
		class RuleGroup
		{
		public:
			RuleGroup(const Rule* rules, int first, int size);

			const Rule* determineNearestRule(const BasicDateTime<>& dt, const Zone& zone, Choose choose);
		private:
			RD calculateFastTransition(const Rule* rule, int transitionYear);
			RD calculateExactTransition(const Rule* rule, const Zone* zone, int transitionYear, TimeType tmType);

			const Rule* m_rules;
			int m_first, m_size;

		};

	}
}
#endif
