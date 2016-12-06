
#include "../include/rule_group.h"
#include "../include/smalltime_exceptions.h"
#include "../include/float_util.h"
#include "../include/time_math.h"
#include <iostream>
#include <assert.h>
#include <array>

namespace smalltime
{
	namespace tz
	{
		enum Transition
		{
			KTransition_MomentBeforeUtc = 0,
			KTransition_MomentBeforeWall = 1,
			KTransition_TransitionWall = 2,
			KTransition_FirstInstWall = 3,
			KTransition_PrevOffset = 4,
			KTransition_CurOffset = 5
		};

		using TransitionData = std::tuple<RD, RD, RD, RD, RD, RD>;

		//=======================================
		// Ctor
		//======================================
		RuleGroup::RuleGroup(uint32_t rule_id, const Zone* const zone, std::shared_ptr<TzdbConnectorInterface> tzdb_connector) : zone_(zone),
			rules_(tzdb_connector->FindRules(rule_id)), rule_arr_(tzdb_connector->GetRuleHandle()), current_year_(0), primary_year_(0),
			previous_year_(0), next_year_(0), primary_ptr_(nullptr), previous_ptr_(nullptr), next_ptr_(nullptr), 
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
			BasicDateTime<> closest_rule_dt(0.0, TimeType::KTimeType_Wall);
			TransitionData closest_rule_data;
			RD closest_diff = DMAX;
			RD diff = 0.0;
			TransitionData transition_data;
			RD mb_any_rd = 0.0;

			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(primary_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];

					transition_data = CalcRuleData(r, primary_year_);
					if (cur_dt.GetType() == KTimeType_Wall)
					{
						mb_any_rd = std::get<KTransition_MomentBeforeWall>(transition_data);
					}
					else if (cur_dt.GetType() == KTimeType_Std)
					{
						mb_any_rd = std::get<KTransition_MomentBeforeUtc>(transition_data) + zone_->zone_offset;
					}
					else
					{
						mb_any_rd = std::get<KTransition_MomentBeforeUtc>(transition_data);
					}

					//auto cur_rule_dt = CalcTransitionAny(r, primary_year_, cur_dt.GetType());
					diff = cur_dt.GetFixed() - mb_any_rd;
					if (diff > 0.0  && diff < closest_diff )
					{
						closest_rule_data = transition_data;
						closest_diff = diff;
						closest_rule = r;
					}
				}
			}
			// if none found in primary pool check previous
			if (closest_rule == nullptr)
			{
				diff = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					// rule transition is not null 
					if (*(previous_ptr_ + i) > 0.0)
					{
						auto r = &rule_arr_[rules_.first + i];

						transition_data = CalcRuleData(r, previous_year_);
						if (cur_dt.GetType() == KTimeType_Wall)
						{
							mb_any_rd = std::get<KTransition_MomentBeforeWall>(transition_data);
						}
						else if (cur_dt.GetType() == KTimeType_Std)
						{
							mb_any_rd = std::get<KTransition_MomentBeforeUtc>(transition_data) + zone_->zone_offset;
						}
						else
						{
							mb_any_rd = std::get<KTransition_MomentBeforeUtc>(transition_data);
						}

						//auto cur_rule_dt = CalcTransitionAny(r, primary_year_, cur_dt.GetType());
						diff = cur_dt.GetFixed() - mb_any_rd;
						if (diff > 0.0  && diff < closest_diff)
						{
							closest_rule_data = transition_data;
							closest_diff = diff;
							closest_rule = r;
						}
					}
				}
			}

			if (closest_rule)
				return CorrectForAmbigAny(cur_dt, closest_rule_data, closest_rule, choose);
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
			BasicDateTime<> closest_rule_dt(0.0, TimeType::KTimeType_Wall);
			RD closest_diff = DMAX;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(primary_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];

					auto cur_rule_dt = CalcTransitionAny(r, primary_year_, cur_dt.GetType());
					diff = cur_dt.GetFixed() - cur_rule_dt.GetFixed();
					if ((diff >= 0.0 || AlmostEqualUlps(cur_dt.GetFixed(), cur_rule_dt.GetFixed(), 11)) && diff < closest_diff)
					{
						closest_rule_dt = cur_rule_dt;
						closest_diff = diff;
						closest_rule = r;
					}
				}
			}
			// if none found in primary pool check previous
			if (closest_rule == nullptr)
			{
				diff = 0.0;
				for (int i = 0; i < rules_.size; ++i)
				{
					// rule transition is not null 
					if (*(previous_ptr_ + i) > 0.0)
					{
						diff = cur_dt.GetFixed() - previous_ptr_[i];
						const tz::Rule* r = &rule_arr_[rules_.first + i];

						auto cur_rule_dt = CalcTransitionAny(r, previous_year_, cur_dt.GetType());
						diff = cur_dt.GetFixed() - cur_rule_dt.GetFixed();

						if ((diff >= 0.0 || AlmostEqualUlps(cur_dt.GetFixed(), cur_rule_dt.GetFixed(), 11)) && diff < closest_diff)
						{
							closest_rule_dt = cur_rule_dt;
							closest_diff = diff;
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
		const Rule* const RuleGroup::CorrectForAmbigAny(const BasicDateTime<>& cur_dt, const std::tuple<RD, RD, RD, RD, RD, RD>& closest_rule_data, const Rule* const cur_rule, Choose choose)
		{
			// Check with current offset for ambiguousness
			RD cur_wall_rd = 0.0;
			if (cur_dt.GetType() == KTimeType_Wall)
				cur_wall_rd = cur_dt.GetFixed();
			else if (cur_dt.GetType() == KTimeType_Std)
				cur_wall_rd = cur_dt.GetFixed() + cur_rule->offset;
			else
				cur_wall_rd = cur_dt.GetFixed() + zone_->zone_offset + cur_rule->offset;
			
			auto rule_transition_wall = std::get<KTransition_TransitionWall>(closest_rule_data);
			
			auto prev_rule = FindPreviousRule(rule_transition_wall);
			if (prev_rule.first)
			{
				auto first_inst_wall = std::get<KTransition_FirstInstWall>(closest_rule_data);
				auto mb_rule_wall = std::get<KTransition_MomentBeforeWall>(closest_rule_data);

				if ((mb_rule_wall <= cur_wall_rd || AlmostEqualUlps(first_inst_wall, cur_wall_rd, 11)) &&
					cur_wall_rd < first_inst_wall)
				{
					// Ambigiuous local time gap
					if (choose == Choose::KEarliest)
						return prev_rule.first;
					else if (choose == Choose::KLatest)
						return cur_rule;
					else
						throw TimeZoneAmbigNoneException(BasicDateTime<>(mb_rule_wall, KTimeType_Wall), BasicDateTime<>(first_inst_wall, KTimeType_Wall));
				}
			}

			auto next_rule = FindNextRule(rule_transition_wall);
			if (next_rule.first)
			{
				auto next_rule_data = CalcRuleData(next_rule.first, next_rule.second);

				auto first_inst_wall = std::get<KTransition_FirstInstWall>(next_rule_data);
				auto mb_rule_wall = std::get<KTransition_MomentBeforeWall>(next_rule_data);

				if ((first_inst_wall <= cur_wall_rd || AlmostEqualUlps(first_inst_wall, cur_wall_rd, 11)) &&
					(cur_wall_rd <= mb_rule_wall || AlmostEqualUlps(mb_rule_wall, cur_wall_rd, 11)))
				{
					// Ambigiuous local time gap
					if (choose == Choose::KEarliest)
						return cur_rule;
					else if (choose == Choose::KLatest)
						return next_rule.first;
					else
						throw TimeZoneAmbigMultiException(BasicDateTime<>(first_inst_wall, KTimeType_Wall), BasicDateTime<>(mb_rule_wall, KTimeType_Wall));
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

		//=======================================================
		// Calculate wall transition in given time type
		//=========================================================
		BasicDateTime<> RuleGroup::CalcTransitionAny(const Rule* const rule, int year, TimeType time_type)
		{
			if (time_type == TimeType::KTimeType_Wall)
				return CalcTransitionWall(rule, year);
			else if (time_type == TimeType::KTimeType_Utc)
				return CalcTransitionUtc(rule, year);
			else
				return CalcTransitionStd(rule, year);
		}

		//============================================
		// Calculate neccesary rule data
		//============================================
		std::tuple<RD, RD, RD, RD, RD, RD> RuleGroup::CalcRuleData(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year);
			if (rule_transition.GetFixed() == 0.0)
				return std::make_tuple(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

			RD cur_rule_offset = rule->offset;
			RD prev_rule_offset = 0.0;

			auto pr = FindPreviousRule(rule_transition);
			if (pr.first)
				prev_rule_offset = pr.first->offset;

			//Calc moment before data
			RD mb_wall_rd = 0.0;
			RD mb_utc_rd = 0.0;
			auto mb_any_rd = rule_transition.GetFixed() - math::MSEC();

			if (rule->at_type == KTimeType_Wall)
			{
				mb_wall_rd = mb_any_rd;
				mb_utc_rd = mb_any_rd - zone_->zone_offset - prev_rule_offset;
			}
			else if (rule->at_type == KTimeType_Std)
			{
				mb_wall_rd = mb_any_rd + prev_rule_offset;;
				mb_utc_rd = mb_any_rd - zone_->zone_offset;
			}
			else
			{
				mb_wall_rd = mb_any_rd + zone_->zone_offset + prev_rule_offset;
				mb_utc_rd = mb_any_rd;
			}

			// Calc transition and first instant data
			RD transition_wall_rd = 0.0;
			RD first_inst_rd = (mb_utc_rd + math::MSEC()) + zone_->zone_offset + cur_rule_offset;
			auto transition_any_rd = rule_transition.GetFixed();

			if (rule->at_type == KTimeType_Wall)
				transition_wall_rd = transition_any_rd;
			else if (rule->at_type == KTimeType_Std)
				transition_wall_rd = transition_any_rd + prev_rule_offset;	
			else
				transition_wall_rd = transition_any_rd + zone_->zone_offset + prev_rule_offset;

			return std::make_tuple(mb_utc_rd, mb_wall_rd, transition_wall_rd, first_inst_rd, prev_rule_offset, cur_rule_offset);

		}

		//==================================================
		// Calculates rule transtion in wall time
		//==================================================
		BasicDateTime<> RuleGroup::CalcTransitionWall(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year);
			if (rule_transition.GetFixed() == 0.0)
				return rule_transition;

			//InitTransitionData(year);

			if (rule->at_type == KTimeType_Wall)
			{
				return rule_transition;
			}
			else if (rule->at_type == KTimeType_Std)
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr.first)
					save = pr.first->offset;

				RD wall_rule_transition = rule_transition.GetFixed() + save;
				return BasicDateTime<>(wall_rule_transition, KTimeType_Wall);

			}
			else
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr.first)
					save = pr.first->offset;

				RD wall_rule_transition = rule_transition.GetFixed() + zone_->zone_offset + save;
				return BasicDateTime<>(wall_rule_transition, KTimeType_Wall);
			}
		}

		//============================================
		// Calculate rule transition in std time
		//=============================================
		BasicDateTime<> RuleGroup::CalcTransitionStd(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year);
			if (rule_transition.GetFixed() == 0.0)
				return rule_transition;

			//InitTransitionData(year);

			if (rule->at_type == KTimeType_Std)
			{
				return rule_transition;
			}
			else if (rule->at_type == KTimeType_Std)
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr.first)
					save = pr.first->offset;

				RD std_rule_transition = rule_transition.GetFixed() - save;
				return BasicDateTime<>(std_rule_transition, KTimeType_Std);

			}
			else
			{

				RD std_rule_transition = rule_transition.GetFixed() + zone_->zone_offset;
				return BasicDateTime<>(std_rule_transition, KTimeType_Std);
			}
		}

		//======================================================
		// Calculate rule transition in utc time
		//======================================================
		BasicDateTime<> RuleGroup::CalcTransitionUtc(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year, KTimeType_Utc);
			if (rule_transition.GetFixed() == 0.0)
				return rule_transition;

			//InitTransitionData(year);
			if (rule->at_type == KTimeType_Utc)
			{
				return rule_transition;
			}
			else if (rule->at_type == KTimeType_Std)
			{
				RD utc_rule_transition = rule_transition.GetFixed() - zone_->zone_offset;
				return BasicDateTime<>(utc_rule_transition, KTimeType_Utc);

			}
			else
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr.first)
					save = pr.first->offset;

				RD utc_rule_transition = rule_transition.GetFixed() - zone_->zone_offset - save;
				return BasicDateTime<>(utc_rule_transition, KTimeType_Utc);
			}

		}

		//===========================================================
		// Calculate rule transiton without time type checking
		//===========================================================
		BasicDateTime<> RuleGroup::CalcTransitionFast(const Rule* const rule, int year, TimeType time_type)
		{
			if (year < rule->from_year || year > rule->to_year)
				return BasicDateTime<>(0.0, time_type);

			//HMS hms = { 0, 0, 0, 0 };
			HMS hms = math::HmsFromFixed(rule->at_time);

			if (rule->day_type == KDayType_Dom)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], time_type);
			else if (rule->day_type == KDayType_SunGE)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::KSunOnOrAfter, time_type);
			else
				return BasicDateTime<>(year, rule->month, 1, hms[0], hms[1], hms[2], hms[3], RelSpec::KLastSun, time_type);
		}
	}
}