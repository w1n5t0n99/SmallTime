
#include "RuleUtil.h"
#include "TzdbAccessor.h"
#include <TimeMath.h>

#include <assert.h>
#include <array>

namespace smalltime
{
	namespace tz
	{

		//=================================================
		// Init rule data for given transition year
		//================================================
		void RuleUtil::InitTransitionYear(int year)
		{
			if (current_year_ == year)
				return;

			current_year_ = year;

			FillIntBuffer(IntBufferType::KRule, 0);
			auto year_buffer = GetIntBuffer(IntBufferType::KRule);

			BuildClosestYearData(year_buffer, year);
			int check_year = FindClosestActiveYear(year_buffer, year);

			FillRDBuffer(RDBufferType::KPrimaryRule, 0.0);
			auto transition_buffer = GetRDBuffer(RDBufferType::KPrimaryRule);
			BuildTransitionData(transition_buffer, check_year);

			FillRDBuffer(RDBufferType::KSecondaryRule, 0.0);
			transition_buffer = GetRDBuffer(RDBufferType::KSecondaryRule);

			FillIntBuffer(IntBufferType::KRule, 0);
			BuildClosestYearData(year_buffer, check_year - 1);
			check_year = FindClosestActiveYear(year_buffer, check_year - 1);
			BuildTransitionData(transition_buffer, check_year - 1);

		}

		//==============================================
		// Find the previous rule in effect if any
		//===============================================
		const Rule* RuleUtil::FindPreviousRule(const Rule* const rule, int year)
		{
			/*const Rule* prevRule = nullptr;
			auto checkTransition = calcFastTransition(rule, year);
			assert(checkTransition > 0.0);

			int checkYear = year;
			setFastTransitions(checkYear);
			prevRule = calcClosestRuleBefore(checkTransition);
			if (prevRule != nullptr)
				return prevRule;

			checkYear = setClosestActiveYears(--checkYear);
			setFastTransitions(checkYear);
			return calcClosestRuleBefore(checkTransition);*/

			const Rule* prev_rule = nullptr;
			auto cur_rule_transition = CalcTransitionFast(rule, year);

			// the rule transition couldn't be calculated
			if (cur_rule_transition.getRd() <= 0.0)
				return prev_rule;

			auto year_buffer = GetIntBuffer(IntBufferType::KRule);
			BuildClosestYearData(year_buffer, year);

			int check_year = FindClosestActiveYear(year_buffer, year);


		}

		//================================================
		// Fill buffer with rule transition data
		//================================================
		void RuleUtil::BuildTransitionData(RD* buffer, int year)
		{
			int index = 0;
			for (int i = rules_.first; i < rules_.size; ++i)
			{
				buffer[index] = CalcTransitionFast(&rule_arr_[i], year).getRd();
				++index;
			}

		}

		//================================================================
		// Fill buffer with each rules closest active year to given year
		//================================================================
		void RuleUtil::BuildClosestYearData(int* buffer, int year)
		{
			int index = 0;
			for (int i = rules_.first; i < rules_.size; ++i)
			{
				int closestYear = 0;
				if (rule_arr_[i].toYear < year)
					closestYear = rule_arr_[i].toYear;
				else if (rule_arr_[i].fromYear > year)
					closestYear = rule_arr_[i].fromYear;
				else
					closestYear = year;

				buffer[index] = closestYear;
				++index;
			}
		}

		//===================================================
		// Calculate rule transiton without time 
		//===================================================
		BasicDateTime<> RuleUtil::CalcTransitionFast(const Rule* const rule, int year)
		{
			if (year < rule->fromYear || year > rule->toYear)
				return BasicDateTime<>(0.0, TimeType_Wall);

			HMS hms = { 0, 0, 0, 0 };

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], TimeType_Wall);
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, TimeType_Wall);
			else
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, TimeType_Wall);
		}

		//================================================================================================
		//================================================================================================
		// Rule id with largest amount of objects
		static const int MAX_RULE_GROUP = 22;

		thread_local static std::array<const Rule*, MAX_RULE_GROUP> VRULES_BUFFER;
		thread_local static std::array<RD, MAX_RULE_GROUP> VDATES_BUFFER;
		thread_local static std::array<int, MAX_RULE_GROUP> VYEARS_BUFFER;

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

			int bufferIndex = 0;
			for (int i = m_group.first; i < m_group.first + m_group.size; ++i)
			{
				auto ruleTransition = calcFastTransition(&m_group.ruleArr[i], transitionYear);
				VRULES_BUFFER[bufferIndex] = &m_group.ruleArr[i];
				VDATES_BUFFER[bufferIndex] = ruleTransition;
				++bufferIndex;
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

			int bufferIndex = 0;
			for (int i = m_group.first; i < m_group.first + m_group.size; ++i)
			{
				auto& rule = m_group.ruleArr[i];

				if (rule.fromYear > year)
					VYEARS_BUFFER[bufferIndex] = rule.fromYear;
				else if (rule.toYear < year)
					VYEARS_BUFFER[bufferIndex] = rule.toYear;
				else
					VYEARS_BUFFER[bufferIndex] = year;

				++bufferIndex;
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