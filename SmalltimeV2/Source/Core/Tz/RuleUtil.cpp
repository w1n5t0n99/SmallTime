
#include "RuleUtil.h"
#include <TimeMath.h>
#include <assert.h>

namespace smalltime
{
	namespace tz
	{
		// Rule id with largest amount of objects
		static int MAX_RULE_GROUP = 22;
		thread_local static std::vector<const Rule*> VRULES_BUFFER(MAX_RULE_GROUP, nullptr);
		thread_local static std::vector<RD> VDATES_BUFFER(MAX_RULE_GROUP, 0.0);
		thread_local static std::vector<int> VYEARS_BUFFER(MAX_RULE_GROUP, 0);

		//=========================================================
		// Return rule transition in wall time for given year
		//=========================================================
		BasicDateTime<> RuleUtil::calcWallTransition(const Rule* rule, int year)
		{
			auto rd = calcFastWithTimeTransition(rule, year);
			if (rd == 0.0)
				return BasicDateTime<>(rd, TimeType_Wall);

			if (rule->atType == TimeType_Wall)
			{
				return BasicDateTime<>(rd, TimeType_Wall);
			}
			else if (rule->atType == TimeType_Std)
			{
				RD save = 0.0;
				auto pr = findPreviousRule(rule, year);

				if (pr)
					save = pr->offset;

				rd += save;
				return BasicDateTime<>(rd, TimeType_Wall);

			}
			else
			{
				RD save = 0.0;
				auto pr = findPreviousRule(rule, year);

				if (pr)
					save = pr->offset;

				rd -= m_zone->zoneOffset;
				rd += save;
				return BasicDateTime<>(rd, TimeType_Wall);
			}

		}

		//=============================================================
		// Find the closest rule before the given rule in given year
		//=============================================================
		const Rule* RuleUtil::findPreviousRule(const Rule* rule, int year)
		{
			const Rule* prevRule = nullptr;
			auto checkTransition = calcFastTransition(rule, year);
			assert(checkTransition > 0.0);

			int checkYear = year;
			setFastTransitions(checkYear);
			prevRule = calcClosestRuleBefore(checkTransition);
			if (prevRule != nullptr)
				return prevRule;

			checkYear = setClosestActiveYears(--checkYear);
			setFastTransitions(checkYear);
			return calcClosestRuleBefore(checkTransition);

		}

		//===================================================================
		// Calculate the fast transitions for all the rules for given year
		//===================================================================
		void RuleUtil::setFastTransitions(int transitionYear)
		{
			std::fill(VRULES_BUFFER.begin(), VRULES_BUFFER.end(), nullptr);
			std::fill(VDATES_BUFFER.begin(), VDATES_BUFFER.end(), 0.0);

			for (int i = m_group.first; i < m_group.size; ++i)
			{
				auto ruleTransition = calcFastTransition(&m_group.ruleArr[i], transitionYear);
				VRULES_BUFFER.push_back(&m_group.ruleArr[i]);
				VDATES_BUFFER.push_back(ruleTransition);
			}
		}

		//==========================================================
		// Find the closest rule before given date but not 0
		//===========================================================
		const Rule* RuleUtil::calcClosestRuleBefore(RD rd)
		{
			
			auto& closest = VRULES_BUFFER[0];
			auto& closestDt = VDATES_BUFFER[0];
			RD diff = 0.0;

			for (int i=0; i <VRULES_BUFFER.size(); ++i)
			{
				diff = rd - VDATES_BUFFER[i];
				// rule transition is not null and before rd
				if (VDATES_BUFFER[i] > 0.0 && diff > 0.0 && VDATES_BUFFER[i] > closestDt)
				{
					closest = VRULES_BUFFER[i];
					closestDt = VDATES_BUFFER[i];
				}
			}

			return closest;
		}

		//==============================================================
		// Find year closest to given year with atleast 1 active rule
		//==============================================================
		int RuleUtil::setClosestActiveYears(int year)
		{
			std::fill(VYEARS_BUFFER.begin(), VYEARS_BUFFER.end(), 0);

			for (int i = 0; i < m_group.size; ++i)
			{
				auto& rule = m_group.ruleArr[m_group.first + i];

				if (rule.fromYear > year)
					VYEARS_BUFFER.push_back(rule.fromYear);
				else if (rule.toYear < year)
					VYEARS_BUFFER.push_back(rule.toYear);
				else
					VYEARS_BUFFER.push_back(year);
			}

			int closestYear = VYEARS_BUFFER[0];

			for (auto y : VYEARS_BUFFER)
			{
				if (y <= year && y > closestYear)
					closestYear = y;
			}

			return closestYear;
		}

		//=======================================================
		// Calculate rule transition to the day only
		//=======================================================
		RD RuleUtil::calcFastTransition(const Rule* rule, int transitionYear)
		{
			if (transitionYear < rule->fromYear || transitionYear > rule->toYear)
				return 0.0;

			HMS hms = { 0, 0, 0, 0 };

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], TimeType_Wall).getRd();
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, TimeType_Wall).getRd();
			else
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, TimeType_Wall).getRd();
		}

		//=======================================================
		// Calculate rule transition to the day and time
		//=======================================================
		RD RuleUtil::calcFastWithTimeTransition(const Rule* rule, int transitionYear)
		{
			if (transitionYear < rule->fromYear || transitionYear > rule->toYear)
				return 0.0;

			HMS hms = math::hmsFromRd(rule->atTime);

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], TimeType_Wall).getRd();
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, TimeType_Wall).getRd();
			else
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, TimeType_Wall).getRd();
		}

	}
}