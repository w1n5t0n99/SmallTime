#include "tzdb_raw_connector.h"
#include <core_math.h>

namespace smalltime
{
	namespace comp
	{
		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto ZONE_CMP = [](const tz::Zones& lhs, const tz::Zones& rhs)
		{
			return lhs.zone_id < rhs.zone_id;
		};

		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto RULE_CMP = [](const tz::Rules& lhs, const tz::Rules& rhs)
		{
			return lhs.rule_id < rhs.rule_id;
		};

		//========================================================
		// Ctor
		//=========================================================
		TzdbRawConnector::TzdbRawConnector(const MetaData& tzdb_meta, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Rule>& vec_rule,
			const std::vector<tz::Zones>& vec_zone_lookup, const std::vector<tz::Rules>& vec_rule_lookup) : meta_data_(tzdb_meta), vec_zone_(vec_zone), vec_rule_(vec_rule),
			vec_zone_lookup_(vec_zone_lookup), vec_rule_lookup_(vec_rule_lookup), zone_pool_(meta_data_.max_zone_size * 3), rule_pool_(meta_data_.max_rule_size * 3)
		{
			
		}

		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const tz::Rule* TzdbRawConnector::GetRuleHandle()
		{
			if (vec_rule_.empty())
				return nullptr;

			return &vec_rule_[0];
		}

		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const tz::Zone* TzdbRawConnector::GetZoneHandle()
		{
			if (vec_zone_.empty())
				return nullptr;

			return &vec_zone_[0];
		}

		//===========================================
		// Get pointer allocated array
		//===========================================
		RD* TzdbRawConnector::GetTransitionPool(const tz::TransitionPool buffer_type)
		{
			if (buffer_type == tz::TransitionPool::KRule)
				return &rule_pool_[0];
			else
				return &zone_pool_[0];
		}

		//========================================
		// Simple fill array with data
		//==========================================
		void TzdbRawConnector::FillTransitionPool(const tz::TransitionPool buffer_type, const RD data)
		{
			if (buffer_type == tz::TransitionPool::KRule)
				std::fill(rule_pool_.begin(), rule_pool_.end(), data);
			else
				std::fill(zone_pool_.begin(), zone_pool_.end(), data);

		}

		//========================================
		// Simple fill array with data
		//==========================================
		void TzdbRawConnector::FillTransitionPool(const tz::TransitionPool buffer_type, int size, const RD data)
		{
			if (buffer_type == tz::TransitionPool::KRule)
				std::fill(rule_pool_.begin(), rule_pool_.begin() + size, data);
			else
				std::fill(zone_pool_.begin(), zone_pool_.begin() + size, data);

		}

		//========================================
		// Simple clear of array with 0.0
		//==========================================
		void TzdbRawConnector::ClearTransitionPool(const tz::TransitionPool buffer_type)
		{
			if (buffer_type == tz::TransitionPool::KRule)
				std::fill(rule_pool_.begin(), rule_pool_.end(), 0.0);
			else
				std::fill(zone_pool_.begin(), zone_pool_.end(), 0.0);

		}

		//========================================
		// Simple clear of array with 0.0
		//==========================================
		void TzdbRawConnector::ClearTransitionPool(const tz::TransitionPool buffer_type, int size)
		{
			if (buffer_type == tz::TransitionPool::KRule)
				std::fill(rule_pool_.begin(), rule_pool_.begin() + size, 0.0);
			else
				std::fill(zone_pool_.begin(), zone_pool_.begin() + size, 0.0);

		}

		//================================================
		// Find rules matching name id
		//================================================
		tz::Rules TzdbRawConnector::FindRules(const std::string& name)
		{
			auto ruleId = math::GetUniqueID(name);
			tz::Rules searchRules = { ruleId, 0, 0 };

			const auto& foundIt = std::lower_bound(vec_rule_lookup_.begin(), vec_rule_lookup_.end(), searchRules, RULE_CMP);

			if (foundIt != vec_rule_lookup_.end())
				return *foundIt;
			else
				return{ ruleId, -1, -1 };
		}

		//================================================
		// Find rules matching name id
		//================================================
		tz::Rules TzdbRawConnector::FindRules(uint32_t ruleId)
		{
			tz::Rules searchRules = { ruleId, 0, 0 };

			const auto& foundIt = std::lower_bound(vec_rule_lookup_.begin(), vec_rule_lookup_.end(), searchRules, RULE_CMP);

			if (foundIt != vec_rule_lookup_.end())
				return *foundIt;
			else
				return{ ruleId, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		tz::Zones TzdbRawConnector::FindZones(const std::string& name)
		{
			auto zoneId = math::GetUniqueID(name);
			tz::Zones searchZones = { zoneId, 0, 0 };

			const auto& foundIt = std::lower_bound(vec_zone_lookup_.begin(), vec_zone_lookup_.end(), searchZones, ZONE_CMP);

			if (foundIt != vec_zone_lookup_.end())
				return *foundIt;
			else
				return{ zoneId, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		tz::Zones TzdbRawConnector::FindZones(uint32_t zoneId)
		{
			tz::Zones searchZones = { zoneId, 0, 0 };

			const auto& foundIt = std::lower_bound(vec_zone_lookup_.begin(), vec_zone_lookup_.end(), searchZones, ZONE_CMP);

			if (foundIt != vec_zone_lookup_.end())
				return *foundIt;
			else
				return{ zoneId, -1, -1 };
		}

	}
}