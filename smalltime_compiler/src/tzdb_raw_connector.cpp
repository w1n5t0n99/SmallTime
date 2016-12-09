#include "../include/tzdb_raw_connector.h"
#include <core_math.h>

namespace smalltime
{
	namespace comp
	{
		
		//===============================================
		// Binary search function for zones
		//===============================================
		tz::Zones BinarySearchZones(uint32_t zone_id, const std::vector<tz::Zones>& vec_zone_lookup)
		{
			int left = 0;
			int right = vec_zone_lookup.size() - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = vec_zone_lookup[middle];

				if (mid_zones.zone_id == zone_id)
					return mid_zones;
				else if (zone_id > mid_zones.zone_id)
					left = middle + 1;
				else
					right = middle - 1;
			}

			return{ zone_id, -1, -1 };
		}

		//===============================================
		// Binary search function for zones
		//===============================================
		tz::Rules BinarySearchRules(uint32_t rule_id, const std::vector<tz::Rules>& vec_rule_lookup)
		{
			int left = 0;
			int right = vec_rule_lookup.size() - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = vec_rule_lookup[middle];

				if (mid_zones.rule_id == rule_id)
					return mid_zones;
				else if (rule_id > mid_zones.rule_id)
					left = middle + 1;
				else
					right = middle - 1;
			}

			return{ rule_id, -1, -1 };
		}

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
			auto rule_id = math::GetUniqueID(name);
			return BinarySearchRules(rule_id, vec_rule_lookup_);
		}

		//================================================
		// Find rules matching name id
		//================================================
		tz::Rules TzdbRawConnector::FindRules(uint32_t rule_id)
		{
			return BinarySearchRules(rule_id, vec_rule_lookup_);
		}

		//================================================
		// Find zones matching name id
		//================================================
		tz::Zones TzdbRawConnector::FindZones(const std::string& name)
		{
			auto zone_id = math::GetUniqueID(name);
			return BinarySearchZones(zone_id, vec_zone_lookup_);
		}

		//================================================
		// Find zones matching name id
		//================================================
		tz::Zones TzdbRawConnector::FindZones(uint32_t zone_id)
		{
			return BinarySearchZones(zone_id, vec_zone_lookup_);

		}

	}
}