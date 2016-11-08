
#include "RuleUtil.h"
#include <TimeMath.h>
#include <assert.h>

namespace smalltime
{
	namespace tz
	{

		//=============================================================
		// Find the closest rule before the given rule in given year
		//=============================================================
		const Rule* RuleUtil::findPreviousRule(const Rule* rule, int year)
		{
			auto checkTransition = calculateFastTransition(rule, year);
			assert(checkTransition > 0.0);

			int checkYear = year;
			m_prevRules.clear();
			//find rule transitions for current year
			for (int i = m_group.first; i < m_group.size; ++i)
			{
				auto ruleTransition = calculateFastTransition(&m_group.ruleArr[i], checkYear);
				m_prevRules.push_back(std::make_pair(&m_group.ruleArr[i], ruleTransition));
			}

			//check if we can find a previosu rule in current year

		}

		//===================================================================
		// Calculate the fast transitions for all the rules for given year
		//===================================================================
		void RuleUtil::calculateAllFastTransitions(const Rule* rule, int transitionYear, bool withTime)
		{

		}

		//==========================================================
		// Find the closest rule before given date but not 0
		//===========================================================
		const Rule* RuleUtil::findClosestRuleBefore(RD rd)
		{
			if (m_prevRules.empty())
				return nullptr;
			
			auto& closest = m_prevRules[0];

			for (auto& r : m_prevRules)
			{
				RD diff = rd - r.second;
				// rule transition is not null and before rd
				if (r.second > 0.0 && diff > 0.0 && r.second > closest.second)
					closest = r;
			}

			return closest.first;
		}

		//=======================================================
		// Calculate rule transition to the day only
		//=======================================================
		RD RuleUtil::calculateFastTransition(const Rule* rule, int transitionYear, bool withTime)
		{
			if (transitionYear < rule->fromYear || transitionYear > rule->toYear)
				return 0.0;

			HMS hms = { 0, 0, 0, 0 };
			if(withTime)
				hms = math::hmsFromRd(rule->atTime);

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], TimeType_Wall).getRd();
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, TimeType_Wall).getRd();
			else
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, TimeType_Wall).getRd();
		}

	}
}