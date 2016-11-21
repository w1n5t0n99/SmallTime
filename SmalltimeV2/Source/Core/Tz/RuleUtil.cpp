
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
			rules_(FindRules(rule_id)), rule_arr_(GetRuleHandle()), current_year_(0), primary_year_(0),
			previous_year_(0), next_year_(0), primary_ptr_(nullptr), previous_ptr_(nullptr), next_ptr_(nullptr)
		{

		}

		//=================================================
		// Init rule data for given transition year
		//================================================
		void RuleUtil::InitTransitionData(int year)
		{
			if (current_year_ == year)
				return;

			current_year_ = year;

			// set the pointers for the data pool
			ClearTransitionPool(TransitionPool::KRule, rules_.size * 3);
			primary_ptr_ = GetTransitionPool(TransitionPool::KRule);
			previous_ptr_ = primary_ptr_ + rules_.size;
			next_ptr_ = previous_ptr_ + rules_.size;
			// set the active years
			primary_year_ = FindClosestActiveYear(year);
			previous_year_ = FindPreviousActiveYear(primary_year_);
			next_year_ = FindNextActiveYear(primary_year_);
			// build transition data for primary year
			BuildTransitionData(primary_ptr_, primary_year_);
			// build transition data for previous closest year
			BuildTransitionData(previous_ptr_, previous_year_);
			// build transition data for next closest year
			BuildTransitionData(next_ptr_, next_year_);

			//std::cout << "prim year: " << primary_year_ << " prev year: " << previous_year_ << " next year: " << next_year_ << std::endl;
		}

		//====================================================
		// Find the active rule if any
		//====================================================
		const Rule* const RuleUtil::FindActiveRule(BasicDateTime<> cur_dt, Choose choose)
		{
			InitTransitionData(cur_dt.getYear());

			const Rule* closest_rule = nullptr;
			BasicDateTime<> closest_rule_dt(0.0, TimeType::TimeType_Wall);
			RD closest_transition = 0.0;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(primary_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];

					closest_rule_dt = CalcTransitionFull(r, primary_year_, cur_dt.getType());
					diff = cur_dt.getRd() - closest_rule_dt.getRd();

					if (diff >= 0.0 && (diff < closest_transition || closest_transition == 0.0))
					{
						closest_transition = diff;
						closest_rule = r;
					}
				}
			}
			// if none found in primary pool check previous
			if (closest_rule == nullptr)
			{
				diff = 0.0;
				closest_transition = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					// rule transition is not null 
					if (*(primary_ptr_ + i) > 0.0)
					{
						diff = cur_dt.getRd() - previous_ptr_[i];
						const tz::Rule* r = &rule_arr_[rules_.first + i];

						closest_rule_dt = CalcTransitionFull(r, previous_year_, cur_dt.getType());
						diff = cur_dt.getRd() - closest_rule_dt.getRd();

						if (diff >= 0.0 && (diff < closest_transition || closest_transition == 0.0))
						{
							closest_transition = diff;
							closest_rule = r;
						}
					}
				}
			}
			
			/*
			if (closest_rule != nullptr)
				return FindCorrectedForAmbigRule(cur_dt, closest_rule_dt, choose);
			else
				return closest_rule;
				*/

			return closest_rule;
		}

		//==============================================
		// Find the previous rule in effect if any
		//===============================================
		std::pair<const Rule* const, int> RuleUtil::FindPreviousRule(BasicDateTime<> cur_rule)
		{
			const Rule* prev_rule = nullptr;
			int prev_rule_year = 0;
			RD closest_transition = 0.0;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				auto rule_rd = *(primary_ptr_ + i);
				diff = cur_rule.getRd() - rule_rd;
				// rule transition is not null and before rd
				if (rule_rd > 0.0 && diff > 0.0 && rule_rd > closest_transition)
				{
					closest_transition = rule_rd;
					prev_rule = &rule_arr_[rules_.first + i];
					prev_rule_year = primary_year_;
				}
			}
			// if a previous rule wasn't found check secondary pool
			if (prev_rule == nullptr)
			{
				diff = 0.0;
				closest_transition = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					auto rule_rd = *(primary_ptr_ + i);
					diff = cur_rule.getRd() - previous_ptr_[i];
					// rule transition is not null and before rd
					if (rule_rd > 0.0 && diff > 0.0 && rule_rd > closest_transition)
					{
						closest_transition = rule_rd;
						prev_rule = &rule_arr_[rules_.first + i];
						prev_rule_year = previous_year_;
					}
				}
			}

			return std::make_pair(prev_rule, prev_rule_year);
		}

		//================================================
		// Find the next rule in effect if any
		//================================================
		std::pair<const Rule* const, int> RuleUtil::FindNextRule(BasicDateTime<> cur_rule)
		{
			const Rule* next_rule = nullptr;
			int next_rule_year = 0;
			RD closest_transition = 0.0;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				auto rule_rd = *(primary_ptr_ + i);
				diff = cur_rule.getRd() - rule_rd;
				// rule transition is not null and after the cur rule
				if (rule_rd > 0.0 && diff < 0.0 && (rule_rd < closest_transition || closest_transition == 0.0))
				{
					closest_transition = rule_rd;
					next_rule = &rule_arr_[rules_.first + i];
					next_rule_year = primary_year_;
				}
			}
			// if a next rule wasn't found check secondary pool
			if (next_rule == nullptr)
			{
				diff = 0.0;
				closest_transition = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					auto rule_rd = *(primary_ptr_ + i);
					diff = cur_rule.getRd() - next_ptr_[i];
					// rule transition is not null and after cur rule
					if (rule_rd > 0.0 && diff < 0.0 && (rule_rd < closest_transition || closest_transition == 0.0))
					{
						closest_transition = rule_rd;
						next_rule = &rule_arr_[rules_.first + i];
						next_rule_year = next_year_;
					}
				}
			}

			return std::make_pair(next_rule, next_rule_year);
		}

		//=========================================================================
		// Check if cur dt is ambigous and choose correct rule or throw exception
		//=========================================================================
		const Rule* const RuleUtil::FindCorrectedForAmbigRule(BasicDateTime<> cur_dt, BasicDateTime<> cur_rule_dt, Choose choose)
		{
			// check for  ambigousness with the next active rule
			auto prev_rule = FindPreviousRule(cur_rule_dt);
			if (prev_rule.first)
			{
				auto prev_rule_offset = prev_rule.first->offset;


			}

			return nullptr;
		}

		//============================================================
		// Find the closest year with an active rule or return 0
		//=============================================================
		int RuleUtil::FindClosestActiveYear(int year)
		{
			int closest_year = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				int rule_range = rule_arr_[i].toYear - rule_arr_[i].fromYear;
				int cur_range = year - rule_arr_[i].fromYear;
				int cur_year = 0;

				if (cur_range > rule_range)
					cur_year = rule_arr_[i].toYear;
				else if (cur_range < 0)
					cur_year = 0;
				else
					cur_year = year;

				if (cur_year > closest_year)
					closest_year = cur_year;
				
			}

			return closest_year;
		}

		//===================================================================
		// Find the closest  previous year with an active rule or return 0
		//===================================================================
		int RuleUtil::FindPreviousActiveYear(int year)
		{
			year -= 1;
			int closest_year = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				int rule_range = rule_arr_[i].toYear - rule_arr_[i].fromYear;
				int cur_range = year - rule_arr_[i].fromYear;
				int cur_year = 0;

				if (cur_range > rule_range)
					cur_year = rule_arr_[i].toYear;
				else if (cur_range < 0)
					cur_year = 0;
				else
					cur_year = year;

				if (cur_year > closest_year)
					closest_year = cur_year;

			}

			return closest_year;
		}

		//============================================================
		// Find the next closest year with an active rule or return 0
		//=============================================================
		int RuleUtil::FindNextActiveYear(int year)
		{
			year += 1;
			int closest_year = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				int rule_range = rule_arr_[i].toYear - rule_arr_[i].fromYear;
				int cur_range = year - rule_arr_[i].fromYear;
				int cur_year = 0;

				if (cur_range > rule_range)
					cur_year = 0;
				else if (cur_range < 0)
					cur_year = rule_arr_[i].fromYear;
				else
					cur_year = year;

				// Set the closest year
				if (closest_year == 0 || cur_year < closest_year)
					closest_year = cur_year;

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

				if (pr.first)
					save = pr.first->offset;

				RD wall_rule_transition = rule_transition.getRd() + save;
				return BasicDateTime<>(wall_rule_transition, TimeType_Wall);

			}
			else
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr.first)
					save = pr.first->offset;

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