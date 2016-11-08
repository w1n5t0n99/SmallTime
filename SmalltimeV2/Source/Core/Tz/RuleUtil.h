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
			RuleUtil(RuleGroup group, const Zone* zone);

			const Rule* findPreviousRule(const Rule* rule, int year);

		private:
			RD calculateFastTransition(const Rule* rule, int transitionYear, bool withTime = false);

			RuleGroup m_group;
			const Zone* zone;

		};
	}
}

#endif
