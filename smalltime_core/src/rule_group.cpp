
#include "../include/rule_group.h"
#include "../include/smalltime_exceptions.h"
#include "../include/float_util.h"
#include "../include/time_math.h"
#include <iostream>
#include <assert.h>
#include <array>
#include <iomanip>

namespace smalltime
{
	namespace tz
	{

		//=======================================
		// Ctor
		//======================================
		RuleGroup::RuleGroup(uint32_t rule_id, const Zone* const zone, const Zone* const prev_zone, std::shared_ptr<TzdbConnectorInterface> tzdb_connector) :
			zone_(zone),
			prev_zone_(prev_zone),
			rules_(tzdb_connector->FindRules(rule_id)),
			rule_arr_(tzdb_connector->GetRuleHandle()), 
			current_year_(0),
			primary_year_(0),
			previous_year_(0),
			next_year_(0),
			primary_ptr_(nullptr),
			previous_ptr_(nullptr),
			next_ptr_(nullptr), 
			zone_transition_(zone_->mb_until_utc, zone_->zone_offset, zone_->next_zone_offset, zone_->mb_rule_offset, zone_->trans_rule_offset),
			prev_zone_transition_(prev_zone == nullptr ? 0.0 : prev_zone->mb_until_utc,
				prev_zone == nullptr ? 0.0 : prev_zone->zone_offset,
				prev_zone == nullptr ? 0.0 : prev_zone->next_zone_offset,
				prev_zone == nullptr ? 0.0 : prev_zone->mb_rule_offset,
				prev_zone == nullptr ? 0.0 : prev_zone->trans_rule_offset),
			tzdb_connector_(tzdb_connector)
		{

		}

		//=================================================
		// Init rule data for given transition year
		//================================================
		void RuleGroup::InitTransitionData(int year)
		{
			if (current_year_ == year)
				return;

			current_year_ = year;

			// set the pointers for the data pool
			tzdb_connector_->ClearTransitionPool(TransitionPool::KRule, rules_.size * 3);
			primary_ptr_ = tzdb_connector_->GetTransitionPool(TransitionPool::KRule);
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
		}

		//====================================================
		// Find the active rule if any
		//====================================================
		const Rule* const RuleGroup::FindActiveRule(BasicDateTime<> cur_dt, Choose choose)
		{
			InitTransitionData(cur_dt.GetYear());

			const Rule* closest_rule = nullptr;
			RuleTransition closest_rule_transition(0.0, 0.0, 0.0, 0.0);

			RD closest_diff = DMAX;
			RD diff = 0.0;

			//RuleTransition cur_rule_transition(0.0, 0.0, 0.0, 0.0);
			RD trans_any = 0.0;

			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(primary_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];
					auto  cur_rule_transition = CalcRuleData(r, primary_year_);

					switch (cur_dt.GetType())
					{
					case KTimeType_Wall:
						trans_any = cur_rule_transition.trans_wall_;
						break;
					case KTimeType_Std:
						trans_any = cur_rule_transition.trans_std_;
						break;
					case KTimeType_Utc:
						trans_any = cur_rule_transition.trans_utc_;
						break;
					}

					diff = cur_dt.GetFixed() - trans_any;
					if ((diff > 0.0 || AlmostEqualUlps(cur_dt.GetFixed(), trans_any, 11)) && diff < closest_diff)
					{
						closest_rule_transition = cur_rule_transition;
						closest_diff = diff;
						closest_rule = r;
					}

				}
			}

			if (closest_rule)
				return CorrectForAmbigAny(cur_dt, closest_rule_transition, closest_rule, choose);

			// Closest rule not found, check previous
			diff = 0.0;
			// check the previous pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(previous_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];
					auto  cur_rule_transition = CalcRuleData(r, previous_year_);

					switch (cur_dt.GetType())
					{
					case KTimeType_Wall:
						trans_any = cur_rule_transition.trans_wall_;
						break;
					case KTimeType_Std:
						trans_any = cur_rule_transition.trans_std_;
						break;
					case KTimeType_Utc:
						trans_any = cur_rule_transition.trans_utc_;
						break;
					}

					diff = cur_dt.GetFixed() - trans_any;
					if ((diff > 0.0 || AlmostEqualUlps(cur_dt.GetFixed(), trans_any, 11)) && diff < closest_diff)
					{
						closest_rule_transition = cur_rule_transition;
						closest_diff = diff;
						closest_rule = r;
					}

				}
			}

			if (closest_rule)
				return CorrectForAmbigAny(cur_dt, closest_rule_transition, closest_rule, choose);
			else
				return closest_rule;
		}

		//==============================================================
		// Find active rulee if any, do not check for ambiguousness
		//==============================================================
		const Rule* const RuleGroup::FindActiveRuleNoCheck(BasicDateTime<> cur_dt)
		{
			InitTransitionData(cur_dt.GetYear());

			const Rule* closest_rule = nullptr;
			RuleTransition closest_rule_transition(0.0, 0.0, 0.0, 0.0);

			RD closest_diff = DMAX;
			RD diff = 0.0;

			//RuleTransition cur_rule_transition(0.0, 0.0, 0.0, 0.0);
			RD trans_any = 0.0;

			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(primary_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];
					auto  cur_rule_transition = CalcRuleData(r, primary_year_);

					switch (cur_dt.GetType())
					{
					case KTimeType_Wall:
						trans_any = cur_rule_transition.trans_wall_;
						break;
					case KTimeType_Std:
						trans_any = cur_rule_transition.trans_std_;
						break;
					case KTimeType_Utc:
						trans_any = cur_rule_transition.trans_utc_;
						break;
					}

					diff = cur_dt.GetFixed() - trans_any;
					if ((diff > 0.0 || AlmostEqualUlps(cur_dt.GetFixed(), trans_any, 11)) && diff < closest_diff)
					{
						closest_rule_transition = cur_rule_transition;
						closest_diff = diff;
						closest_rule = r;
					}

				}
			}

			if (closest_rule)
				return closest_rule;

			// Closest rule not found, check previous
			diff = 0.0;
			// check the previous pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(previous_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];
					auto  cur_rule_transition = CalcRuleData(r, previous_year_);

					switch (cur_dt.GetType())
					{
					case KTimeType_Wall:
						trans_any = cur_rule_transition.trans_wall_;
						break;
					case KTimeType_Std:
						trans_any = cur_rule_transition.trans_std_;
						break;
					case KTimeType_Utc:
						trans_any = cur_rule_transition.trans_utc_;
						break;
					}

					diff = cur_dt.GetFixed() - trans_any;
					if ((diff > 0.0 || AlmostEqualUlps(cur_dt.GetFixed(), trans_any, 11)) && diff < closest_diff)
					{
						closest_rule_transition = cur_rule_transition;
						closest_diff = diff;
						closest_rule = r;
					}

				}
			}

			return closest_rule;
		}

		//==============================================
		// Find the previous rule in effect if any
		//===============================================
		std::pair<const Rule* const, int> RuleGroup::FindPreviousRule(BasicDateTime<> cur_rule)
		{
			const Rule* prev_rule = nullptr;
			int prev_rule_year = 0;
			RD closest_diff = 0.0;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				auto rule_rd = *(primary_ptr_ + i);
				diff = cur_rule.GetFixed() - rule_rd;
				// rule transition is not null and before rd
				if (rule_rd > 0.0 && diff > 0.0 && rule_rd > closest_diff)
				{
					closest_diff = rule_rd;
					prev_rule = &rule_arr_[rules_.first + i];
					prev_rule_year = primary_year_;
				}
			}
			// if a previous rule wasn't found check secondary pool
			if (prev_rule == nullptr)
			{
				diff = 0.0;
				closest_diff = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					auto rule_rd = *(previous_ptr_ + i);
					diff = cur_rule.GetFixed() - rule_rd;
					// rule transition is not null and before rd
					// rule transition will not be no way close enough to be within ulp margin of error
					if (rule_rd > 0.0 && diff > 0.0 && rule_rd > closest_diff)
					{
						closest_diff = rule_rd;
						prev_rule = &rule_arr_[rules_.first + i];
						prev_rule_year = previous_year_;
					}
				}
			}

			return std::make_pair(prev_rule, prev_rule_year);
		}

		//==============================================
		// Find the previous rule in effect if any
		//===============================================
		std::pair<const Rule* const, int> RuleGroup::FindPreviousRule(RD cur_rule)
		{
			const Rule* prev_rule = nullptr;
			int prev_rule_year = 0;
			RD closest_diff = 0.0;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				auto rule_rd = *(primary_ptr_ + i);
				diff = cur_rule - rule_rd;
				// rule transition is not null and before rd
				if (rule_rd > 0.0 && diff > 0.0 && rule_rd > closest_diff)
				{
					closest_diff = rule_rd;
					prev_rule = &rule_arr_[rules_.first + i];
					prev_rule_year = primary_year_;
				}
			}
			// if a previous rule wasn't found check secondary pool
			if (prev_rule == nullptr)
			{
				diff = 0.0;
				closest_diff = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					auto rule_rd = *(previous_ptr_ + i);
					diff = cur_rule - rule_rd;
					// rule transition is not null and before rd
					// rule transition will not be no way close enough to be within ulp margin of error
					if (rule_rd > 0.0 && diff > 0.0 && rule_rd > closest_diff)
					{
						closest_diff = rule_rd;
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
		std::pair<const Rule* const, int> RuleGroup::FindNextRule(BasicDateTime<> cur_rule)
		{
			const Rule* next_rule = nullptr;
			int next_rule_year = 0;
			RD closest_diff = DMAX;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				auto rule_rd = *(primary_ptr_ + i);
				diff = cur_rule.GetFixed() - rule_rd;
				// rule transition is not null and after the cur rule
				// rule transition will not be no way close enough to be within ulp margin of error
				if (rule_rd > 0.0 && diff < 0.0 && rule_rd < closest_diff)
				{
					closest_diff = rule_rd;
					next_rule = &rule_arr_[rules_.first + i];
					next_rule_year = primary_year_;
				}
			}
			// if a next rule wasn't found check secondary pool
			if (next_rule == nullptr)
			{
				diff = 0.0;
				closest_diff = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					auto rule_rd = *(next_ptr_ + i);
					diff = cur_rule.GetFixed() - rule_rd;
					// rule transition is not null and after cur rule
					if (rule_rd > 0.0 && diff < 0.0 && rule_rd < closest_diff )
					{
						closest_diff = rule_rd;
						next_rule = &rule_arr_[rules_.first + i];
						next_rule_year = next_year_;
					}
				}
			}

			return std::make_pair(next_rule, next_rule_year);
		}

		//================================================
		// Find the next rule in effect if any
		//================================================
		std::pair<const Rule* const, int> RuleGroup::FindNextRule(RD cur_rule)
		{
			const Rule* next_rule = nullptr;
			int next_rule_year = 0;
			RD closest_diff = DMAX;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				auto rule_rd = *(primary_ptr_ + i);
				diff = cur_rule - rule_rd;
				// rule transition is not null and after the cur rule
				// rule transition will not be no way close enough to be within ulp margin of error
				if (rule_rd > 0.0 && diff < 0.0 && rule_rd < closest_diff)
				{
					closest_diff = rule_rd;
					next_rule = &rule_arr_[rules_.first + i];
					next_rule_year = primary_year_;
				}
			}
			// if a next rule wasn't found check secondary pool
			if (next_rule == nullptr)
			{
				diff = 0.0;
				closest_diff = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					auto rule_rd = *(next_ptr_ + i);
					diff = cur_rule - rule_rd;
					// rule transition is not null and after cur rule
					if (rule_rd > 0.0 && diff < 0.0 && rule_rd < closest_diff)
					{
						closest_diff = rule_rd;
						next_rule = &rule_arr_[rules_.first + i];
						next_rule_year = next_year_;
					}
				}
			}

			return std::make_pair(next_rule, next_rule_year);
		}

		//==========================================================================
		// check if the cur date time is within an ambiguous range in wall time
		//==========================================================================
		const Rule* const RuleGroup::CorrectForAmbigAny(const BasicDateTime<>& cur_dt, RuleTransition cur_rule_transition, const Rule* const cur_rule, Choose choose)
		{
			
			// Zone and Rule transition are the same, zone will have already checked for ambig
			// Return the current rule since were assuming its not ambiguous or the zone would have caught it
			if (AlmostEqualUlps(prev_zone_transition_.trans_wall_, cur_rule_transition.trans_wall_, 11))
				return cur_rule;

			auto prev_rule = FindPreviousRule(cur_rule_transition.trans_wall_);
			if (prev_rule.first)
			{
				RD mb_any = 0.0;
				RD fi_any = 0.0;
				TimeType time_type = KTimeType_Wall;

				switch (cur_dt.GetType())
				{
				case KTimeType_Wall:
					mb_any = cur_rule_transition.mb_trans_wall_;
					fi_any = cur_rule_transition.first_inst_wall_;
					time_type = KTimeType_Wall;
					break;
				case KTimeType_Std:
					mb_any = cur_rule_transition.mb_trans_std_;
					fi_any = cur_rule_transition.first_inst_std_;
					time_type = KTimeType_Std;
					break;
				case KTimeType_Utc:
					mb_any = cur_rule_transition.mb_trans_utc_;
					fi_any = cur_rule_transition.trans_utc_;
					time_type = KTimeType_Utc;
					break;
				}

				// check for ambig with previous rule and current rule
				if (mb_any < cur_dt.GetFixed() && cur_dt.GetFixed() < fi_any)
				{
					switch (choose)
					{
					// Ambigiuous local time gap
					case Choose::KEarliest:
						return prev_rule.first;
					case Choose::KLatest:
						return cur_rule;
					case Choose::KError:
						throw TimeZoneAmbigNoneException(BasicDateTime<>(mb_any, time_type), BasicDateTime<>(fi_any, time_type));
					}
				}
			}

			// check for ambig with cur rule and next rule
			auto next_rule = FindNextRule(cur_rule_transition.trans_wall_);
			if (next_rule.first)
			{
				RD mb_any = 0.0;
				RD fi_any = 0.0;
				TimeType time_type = KTimeType_Wall;

				RuleTransition next_rule_transition = CalcRuleData(next_rule.first, next_rule.second);
				// Zone and Rule transition are the same, zone will have already checked for ambig
				// Return the current rule since were assuming its not ambiguous or the zone would have caught it
				if (AlmostEqualUlps(zone_transition_.trans_wall_, next_rule_transition.trans_wall_, 11))
					return cur_rule;

				switch (cur_dt.GetType())
				{
				case KTimeType_Wall:
					mb_any = next_rule_transition.mb_trans_wall_;
					fi_any = next_rule_transition.first_inst_wall_;
					time_type = KTimeType_Wall;
					break;
				case KTimeType_Std:
					mb_any = next_rule_transition.mb_trans_std_;
					fi_any = next_rule_transition.first_inst_std_;
					time_type = KTimeType_Std;
					break;
				case KTimeType_Utc:
					mb_any = next_rule_transition.mb_trans_utc_;
					fi_any = next_rule_transition.trans_utc_;
					time_type = KTimeType_Utc;
					break;
				}

				// check for ambig with current and next rule
				if ((fi_any <= cur_dt.GetFixed() || AlmostEqualUlps(fi_any, cur_dt.GetFixed(), 11)) &&
					(cur_dt.GetFixed() <= mb_any || AlmostEqualUlps(mb_any, cur_dt.GetFixed(), 11)))
				{
					// Ambigiuous local time gap
					switch (choose)
					{
					case Choose::KEarliest:
						return cur_rule;
					case Choose::KLatest:
						return next_rule.first;
					case Choose::KError:
						throw TimeZoneAmbigMultiException(BasicDateTime<>(fi_any, time_type), BasicDateTime<>(mb_any, time_type));
					}
				}
			}

			return cur_rule;
		}

		//============================================================
		// Find the closest year with an active rule or return 0
		//=============================================================
		int RuleGroup::FindClosestActiveYear(int year)
		{
			int closest_year = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				int rule_range = rule_arr_[i].to_year - rule_arr_[i].from_year;
				int cur_range = year - rule_arr_[i].from_year;
				int cur_year = 0;

				if (cur_range > rule_range)
					cur_year = rule_arr_[i].to_year;
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
		int RuleGroup::FindPreviousActiveYear(int year)
		{
			year -= 1;
			int closest_year = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				int rule_range = rule_arr_[i].to_year - rule_arr_[i].from_year;
				int cur_range = year - rule_arr_[i].from_year;
				int cur_year = 0;

				if (cur_range > rule_range)
					cur_year = rule_arr_[i].to_year;
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
		int RuleGroup::FindNextActiveYear(int year)
		{
			year += 1;
			int closest_year = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				int rule_range = rule_arr_[i].to_year - rule_arr_[i].from_year;
				int cur_range = year - rule_arr_[i].from_year;
				int cur_year = 0;

				if (cur_range > rule_range)
					cur_year = 0;
				else if (cur_range < 0)
					cur_year = rule_arr_[i].from_year;
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
		void RuleGroup::BuildTransitionData(RD* buffer, int year)
		{
			int index = 0;
			for (int i = rules_.first; i < rules_.first + rules_.size; ++i)
			{
				buffer[index] = CalcTransitionFast(&rule_arr_[i], year).GetFixed();
				++index;
			}

		}

		//============================================
		// Calculate neccesary rule data
		//============================================
		RuleTransition RuleGroup::CalcRuleData(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year);
			if (rule_transition.GetFixed() == 0.0)
				return RuleTransition(0.0, 0.0, 0.0, 0.0);

			RD zone_offset = zone_->zone_offset;
			RD cur_rule_offset = rule->offset;
			RD prev_rule_offset = 0.0;

			auto pr = FindPreviousRule(rule_transition);
			if (pr.first)
				prev_rule_offset = pr.first->offset;

			RD mb_trans_utc = 0.0;

			switch (rule_transition.GetType())
			{
			case KTimeType_Wall:
				mb_trans_utc = rule_transition.GetFixed() - prev_rule_offset - zone_offset - math::MSEC();
				break;
			case KTimeType_Std:
				mb_trans_utc = rule_transition.GetFixed() - zone_offset - math::MSEC();
				break;
			case KTimeType_Utc:
				mb_trans_utc =  math::MSEC();
				break;
			}

			return RuleTransition(mb_trans_utc, zone_offset, cur_rule_offset, prev_rule_offset);

		}

		//===========================================================
		// Calculate rule transiton without time type checking
		//===========================================================
		BasicDateTime<> RuleGroup::CalcTransitionFast(const Rule* const rule, int year)
		{
			if (year < rule->from_year || year > rule->to_year)
				return BasicDateTime<>(0.0, rule->at_type);

			//HMS hms = { 0, 0, 0, 0 };
			HMS hms = math::HmsFromFixed(rule->at_time);
			TimeType time_type = rule->at_type;

			switch (rule->day_type)
			{
			case KDayType_Dom:
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], time_type);
			case KDayType_SunGE:
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::KSunOnOrAfter, time_type);
			case KDayType_LastSun:
				return BasicDateTime<>(year, rule->month, 1, hms[0], hms[1], hms[2], hms[3], RelSpec::KLastSun, time_type);
			}
		}
	}
}