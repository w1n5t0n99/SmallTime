
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
			const Rule* prevRule = nullptr;
			auto checkTransition = calculateFastTransition(rule, year);
			assert(checkTransition > 0.0);

			int checkYear = year;
			calculateAllFastTransitions(checkYear);
			prevRule = findClosestRuleBefore(checkTransition);
			if (prevRule != nullptr)
				return prevRule;

			checkYear = findClosestYearWithActiveRules(--checkYear);
			calculateAllFastTransitions(checkYear);
			return findClosestRuleBefore(checkTransition);

		}

		//===================================================================
		// Calculate the fast transitions for all the rules for given year
		//===================================================================
		void RuleUtil::calculateAllFastTransitions(int transitionYear)
		{
			m_prevRules.clear();

			for (int i = m_group.first; i < m_group.size; ++i)
			{
				auto ruleTransition = calculateFastTransition(&m_group.ruleArr[i], transitionYear);
				m_prevRules.push_back(std::make_pair(&m_group.ruleArr[i], ruleTransition));
			}
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

		//==============================================================
		// Find year closest to given year with atleast 1 active rule
		//==============================================================
		int RuleUtil::findClosestYearWithActiveRules(int year)
		{
			std::vector<int> closestYears = {};
			closestYears.reserve(m_group.size + 1);
			closestYears.push_back(0);

			for (int i = m_group.first; i < m_group.size; ++i)
			{
				if (year > m_group.ruleArr[i].toYear)
					closestYears.push_back(m_group.ruleArr[i].toYear);
				else if (year < m_group.ruleArr[i].fromYear)
					closestYears.push_back(0);
				else
					closestYears.push_back(year);					
			}

			int retY = closestYears[0];
			for (auto y : closestYears)
			{
				if (y > retY)
					retY = y;
			}

			return retY;

		}

		//=======================================================
		// Calculate rule transition to the day only
		//=======================================================
		RD RuleUtil::calculateFastTransition(const Rule* rule, int transitionYear)
		{
			if (transitionYear < rule->fromYear || transitionYear > rule->toYear)
				return 0.0;

			HMS hms = { 0, 0, 0, 0 };
			//if(withTime)
			//	hms = math::hmsFromRd(rule->atTime);

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], TimeType_Wall).getRd();
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, TimeType_Wall).getRd();
			else
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, TimeType_Wall).getRd();
		}

	}
}