
#include "rule_group.h"
#include "smalltime_exceptions.h"
#include "floatutil.h"
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
		RuleGroup::RuleGroup(uint32_t rule_id, const Zone* const zone, std::shared_ptr<ITzdbConnector> tzdb_connector) : zone_(zone),
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
			InitTransitionData(cur_dt.getYear());

			const Rule* closest_rule = nullptr;
			BasicDateTime<> closest_rule_dt(0.0, TimeType::TimeType_Wall);
			RD closest_diff = DMAX;
			RD diff = 0.0;
			// check the primary pool for an active rule 
			for (int i = 0; i < rules_.size; ++i)
			{
				// rule transition is not null 
				if (*(primary_ptr_ + i) > 0.0)
				{
					auto r = &rule_arr_[rules_.first + i];

					auto cur_rule_dt = CalcTransitionWallOrUtc(r, primary_year_, cur_dt.getType());
					diff = cur_dt.getRd() - cur_rule_dt.getRd();
					if ((diff >= 0.0  || AlmostEqualUlps(cur_dt.getRd(), cur_rule_dt.getRd(), 11)) && diff < closest_diff )
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
						diff = cur_dt.getRd() - previous_ptr_[i];
						const tz::Rule* r = &rule_arr_[rules_.first + i];

						auto cur_rule_dt = CalcTransitionWallOrUtc(r, previous_year_, cur_dt.getType());
						diff = cur_dt.getRd() - cur_rule_dt.getRd();

						if ((diff >= 0.0 || AlmostEqualUlps(cur_dt.getRd(), cur_rule_dt.getRd(), 11)) && diff < closest_diff)
						{
							closest_rule_dt = cur_rule_dt;
							closest_diff = diff;
							closest_rule = r;
						}
					}
				}
			}

			if (closest_rule)
				return CorrectForAmbigWallOrUtc(cur_dt, closest_rule_dt, closest_rule, choose);
			else
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
				diff = cur_rule.getRd() - rule_rd;
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
					diff = cur_rule.getRd() - rule_rd;
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
				diff = cur_rule.getRd() - rule_rd;
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
					diff = cur_rule.getRd() - rule_rd;
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

		//==========================================================================
		// check if the cur date time is within an ambiguous range in wall time
		//==========================================================================
		const Rule* const RuleGroup::CorrectForAmbigWallOrUtc(const BasicDateTime<>& cur_dt, const BasicDateTime<>& cur_rule_dt, const Rule* const cur_rule, Choose choose)
		{
			// Check with previous offset for ambiguousness
			auto prev_rule = FindPreviousRule(cur_rule_dt);
			if (prev_rule.first)
			{
				auto offset_diff = cur_rule->offset - prev_rule.first->offset;

				RD cur_rule_inst = 0.0;
				if(cur_dt.getType() == TimeType_Wall)
					cur_rule_inst = cur_rule_dt.getRd() + offset_diff;
				else if(cur_dt.getType() == TimeType_Utc)
					cur_rule_inst = cur_rule_dt.getRd() - offset_diff;

				if ((cur_dt.getRd() >= cur_rule_dt.getRd() || AlmostEqualUlps(cur_dt.getRd(), cur_rule_dt.getRd(), 11))&& cur_dt.getRd() < cur_rule_inst)
				{
					// Ambigiuous local time gap
					if (choose == Choose::Earliest)
					{
						return prev_rule.first;
					}
					else if (choose == Choose::Latest)
					{
						return cur_rule;
					}
					else
					{
						if (cur_dt.getType() == TimeType_Wall)
							throw TimeZoneAmbigNoneException(cur_rule_dt, BasicDateTime<>(cur_rule_inst - math::MSEC(), TimeType_Wall));
						else if (cur_dt.getType() == TimeType_Utc)
							throw TimeZoneAmbigMultiException(cur_rule_dt, BasicDateTime<>(cur_rule_inst - math::MSEC(), TimeType_Utc));
					}
				}
			}

			// check for ambiguousness with next active rule
			auto next_rule = FindNextRule(cur_rule_dt);
			if (next_rule.first)
			{
				// Thier cannot be a gap in UTC so no need to check!
				auto next_rule_dt = CalcTransitionWallOrUtc(next_rule.first, next_rule.second, cur_rule_dt.getType());
				auto offset_diff = next_rule.first->offset - cur_rule->offset;
				auto next_rule_inst = next_rule_dt.getRd() + offset_diff;

				if ((cur_dt.getRd() >= next_rule_inst || AlmostEqualUlps(cur_dt.getRd(), next_rule_inst, 11)) && cur_dt.getRd() < next_rule_dt.getRd())
				{
					// Ambiguous multiple local times
					if (choose == Choose::Earliest)
						return cur_rule;
					else if (choose == Choose::Latest)
						return next_rule.first;
					else
						throw TimeZoneAmbigMultiException(BasicDateTime<>(next_rule_inst - math::MSEC(), TimeType_Wall), next_rule_dt);
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
		int RuleGroup::FindPreviousActiveYear(int year)
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
		int RuleGroup::FindNextActiveYear(int year)
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
		void RuleGroup::BuildTransitionData(RD* buffer, int year)
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
		BasicDateTime<> RuleGroup::CalcTransitionWallOrUtc(const Rule* const rule, int year, TimeType time_type)
		{
			if (time_type == TimeType::TimeType_Wall)
				return CalcTransitionWall(rule, year);
			else if (time_type == TimeType::TimeType_Utc)
				return CalcTransitionUtc(rule, year);
			else
				return BasicDateTime<>(0.0, TimeType_Std);
		}

		//==================================================
		// Calculates rule transtion in wall time
		//==================================================
		BasicDateTime<> RuleGroup::CalcTransitionWall(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year);
			if (rule_transition.getRd() == 0.0)
				return rule_transition;

			//InitTransitionData(year);

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

				RD wall_rule_transition = rule_transition.getRd() + zone_->zoneOffset + save;
				return BasicDateTime<>(wall_rule_transition, TimeType_Wall);
			}
		}

		//======================================================
		// Calculate rule transition in utc time
		//======================================================
		BasicDateTime<> RuleGroup::CalcTransitionUtc(const Rule* const rule, int year)
		{
			auto rule_transition = CalcTransitionFast(rule, year, TimeType_Utc);
			if (rule_transition.getRd() == 0.0)
				return rule_transition;

			//InitTransitionData(year);
			if (rule->atType == TimeType_Utc)
			{
				return rule_transition;
			}
			else if (rule->atType == TimeType_Std)
			{
				RD utc_rule_transition = rule_transition.getRd() - zone_->zoneOffset;
				return BasicDateTime<>(utc_rule_transition, TimeType_Utc);

			}
			else
			{
				RD save = 0.0;
				auto pr = FindPreviousRule(rule_transition);

				if (pr.first)
					save = pr.first->offset;

				RD utc_rule_transition = rule_transition.getRd() - zone_->zoneOffset - save;
				return BasicDateTime<>(utc_rule_transition, TimeType_Utc);
			}

		}

		//===========================================================
		// Calculate rule transiton without time type checking
		//===========================================================
		BasicDateTime<> RuleGroup::CalcTransitionFast(const Rule* const rule, int year, TimeType time_type)
		{
			if (year < rule->fromYear || year > rule->toYear)
				return BasicDateTime<>(0.0, time_type);

			//HMS hms = { 0, 0, 0, 0 };
			HMS hms = math::hmsFromRd(rule->atTime);

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], time_type);
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, time_type);
			else
				return BasicDateTime<>(year, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, time_type);
		}
	}
}