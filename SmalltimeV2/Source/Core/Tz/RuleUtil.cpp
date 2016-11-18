
#include "RuleUtil.h"
#include "TzdbAccessor.h"
#include <TimeMath.h>
#include <iostream>
#include <assert.h>
#include <array>

namespace smalltime
{
	namespace tz
	{
		//=======================================
		// Ctor
		//======================================
		RuleUtil::RuleUtil(uint32_t rule_id, const Zone* const zone) : zone_(zone),
			rules_(FindRules(rule_id)), rule_arr_(GetRuleHandle()), current_year_(0), primary_pool_year_(0),
			secondary_pool_year_(0)
		{

		}

		//=================================================
		// Init rule data for given transition year
		//================================================
		void RuleUtil::InitTransitionData(int year)
		{
			if (current_year_ == year)
				return;

			ClearTransitionPool(TransitionPool::KPrimaryRule);
			ClearTransitionPool(TransitionPool::KSecondaryRule);

			current_year_ = year;
			int check_year = FindClosestActiveYear(year);

			auto transition_buffer = GetTransitionPool(TransitionPool::KPrimaryRule);
			BuildTransitionData(transition_buffer, check_year);
			primary_pool_year_ = check_year;

			check_year = FindClosestActiveYear(check_year - 1);

			transition_buffer = GetTransitionPool(TransitionPool::KSecondaryRule);
			BuildTransitionData(transition_buffer, check_year);
			secondary_pool_year_ = check_year;


		}

		//====================================================
		// Find the active rule if any
		//====================================================
		const Rule* const RuleUtil::FindActiveRule(BasicDateTime<> cur_dt)
		{
			InitTransitionData(cur_dt.getYear());

			const Rule* closest_rule = nullptr;
			auto transition_pool = GetTransitionPool(TransitionPool::KPrimaryRule);
			RD closest_transition = 0.0;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < GetMaxRuleSize(); ++i)
			{
				// rule transition is not null 
				if (transition_pool[i] > 0.0)
				{
					diff = cur_dt.getRd() - transition_pool[i];
					const tz::Rule* r = &rule_arr_[rules_.first + i];
					diff = cur_dt.getRd() - CalcTransitionFull(r, primary_pool_year_, cur_dt.getType()).getRd();

					if (diff >= 0.0 && diff < closest_transition)
					{
						closest_transition = diff;
						closest_rule = r;
					}
				}
			}
			// if none found in primary pool check secondary
			if (closest_rule == nullptr)
			{
				transition_pool = GetTransitionPool(TransitionPool::KSecondaryRule);
				diff = 0.0;
				closest_transition = 0.0;
				for (int i = 0; i < GetMaxRuleSize(); ++i)
				{
					// rule transition is not null 
					if (transition_pool[i] > 0.0)
					{
						diff = cur_dt.getRd() - transition_pool[i];
						const tz::Rule* r = &rule_arr_[rules_.first + i];
						diff = cur_dt.getRd() - CalcTransitionFull(r, primary_pool_year_, cur_dt.getType()).getRd();

						if (diff >= 0.0 && diff < closest_transition)
						{
							closest_transition = diff;
							closest_rule = r;
						}
					}
				}
			}
			
			return closest_rule;
		}

		//==============================================
		// Find the previous rule in effect if any
		//===============================================
		const Rule* const RuleUtil::FindPreviousRule(BasicDateTime<> cur_rule)
		{
			const Rule* prev_rule = nullptr;

			auto transition_pool = GetTransitionPool(TransitionPool::KPrimaryRule);
			RD closest_transition = 0.0;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < GetMaxRuleSize(); ++i)
			{
				diff = cur_rule.getRd() - transition_pool[i];
				// rule transition is not null and before rd
				if (transition_pool[i] > 0.0 && diff > 0.0 && transition_pool[i] > closest_transition)
				{
					closest_transition = transition_pool[i];
					prev_rule = &rule_arr_[rules_.first + i];
				}
			}
			// if a previous rule wasn't found check secondary pool
			if (prev_rule == nullptr)
			{
				transition_pool = GetTransitionPool(TransitionPool::KSecondaryRule);
				diff = 0.0;
				closest_transition = 0.0;
				for (int i = 0; i < GetMaxRuleSize(); ++i)
				{
					diff = cur_rule.getRd() - transition_pool[i];
					// rule transition is not null and before rd
					if (transition_pool[i] > 0.0 && diff > 0.0 && transition_pool[i] > closest_transition)
					{
						closest_transition = transition_pool[i];
						prev_rule = &rule_arr_[rules_.first + i];
					}
				}
			}

			return prev_rule;
		}

		//============================================================
		// Find the closest year with an active rule or return 0
		//=============================================================
		int RuleUtil::FindClosestActiveYear(int year)
		{
			int closest_year = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				if (rule_arr_[i].fromYear <= year && rule_arr_[i].toYear >= year)
				{
					closest_year = year;
				}
				else if (rule_arr_[i].toYear < year)
				{
					if (rule_arr_[i].toYear > closest_year)
						closest_year = rule_arr_[i].toYear;
				}
			}

			return closest_year;
		}

		//================================================
		// Fill buffer with rule transition data
		//================================================
		void RuleUtil::BuildTransitionData(RD* buffer, int year)
		{
			int index = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				buffer[index] = CalcTransitionFast(&rule_arr_[i], year).getRd();
				++index;
			}

		}

		//=======================================================
		// Calculate wall transition in given time type
		//=========================================================
		BasicDateTime<> RuleUtil::CalcTransitionFull(const Rule* const rule, int year, TimeType time_type)
		{
			//if (time_type == TimeType::TimeType_Wall)
				return CalcTransitionWall(rule, year);
		}

		//==================================================
		// Calculates rule transtion in wall time
		//==================================================
		BasicDateTime<> RuleUtil::CalcTransitionWall(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year);
			if (rule_transition.getRd() == 0.0)
				return rule_transition;

			InitTransitionData(year);

			if (rule->atType == TimeType_Wall)
			{
				return rule_transition;
			}
			else if (rule->atType == TimeType_Std)
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr)
					save = pr->offset;

				RD wall_rule_transition = rule_transition.getRd() + save;
				return BasicDateTime<>(wall_rule_transition, TimeType_Wall);

			}
			else
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr)
					save = pr->offset;

				RD wall_rule_transition = rule_transition.getRd() - zone_->zoneOffset;
				wall_rule_transition += save;
				return BasicDateTime<>(wall_rule_transition, TimeType_Wall);
			}
		}

		//===================================================
		// Calculate rule transiton without time 
		//===================================================
		BasicDateTime<> RuleUtil::CalcTransitionFast(const Rule* const rule, int year)
		{
			if (year < rule->fromYear || year > rule->toYear)
				return BasicDateTime<>(0.0, TimeType_Wall);

			//HMS hms = { 0, 0, 0, 0 };
			HMS hms = math::hmsFromRd(rule->atTime);

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], TimeType_Wall);
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, TimeType_Wall);
			else
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, TimeType_Wall);
		}
	}
}