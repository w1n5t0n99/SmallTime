#include "../include/tzdb_header_connector.h"
#include "../include/Tzdb.h"
#include <core_math.h>

namespace smalltime
{
	namespace tz
	{
	
		//===============================================
		// Binary search function for zones
		//===============================================
		Zones BinarySearchZones(uint32_t zone_id)
		{
			int left = 0;
			int right = KZoneLookupArray.size() - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = KZoneLookupArray[middle];

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
		Rules BinarySearchRules(uint32_t rule_id)
		{
			int left = 0;
			int right = KRuleLookupArray.size() - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = KRuleLookupArray[middle];

				if (mid_zones.rule_id == rule_id)
					return mid_zones;
				else if (rule_id > mid_zones.rule_id)
					left = middle + 1;
				else
					right = middle - 1;
			}

			return{ rule_id, -1, -1 };
		}

		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const Rule* TzdbHeaderConnector::GetRuleHandle()
		{
			return KRuleArray.data();
		}

		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const Zone* TzdbHeaderConnector::GetZoneHandle()
		{
			return KZoneArray.data();
		}

		//===========================================
		// Get pointer allocated array
		//===========================================
		RD* TzdbHeaderConnector::GetTransitionPool(const TransitionPool buffer_type)
		{
			if (buffer_type == TransitionPool::KRule)
				return rule_pool.data();
			else
				return zone_pool.data();
		}

		//========================================
		// Simple fill array with data
		//==========================================
		void TzdbHeaderConnector::FillTransitionPool(const TransitionPool buffer_type, const RD data)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(rule_pool.begin(), rule_pool.end(), data);
			else
				std::fill(zone_pool.begin(), zone_pool.end(), data);

		}

		//========================================
		// Simple fill array with data
		//==========================================
		void TzdbHeaderConnector::FillTransitionPool(const TransitionPool buffer_type, int size, const RD data)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(rule_pool.begin(), rule_pool.begin() + size, data);
			else
				std::fill(zone_pool.begin(), zone_pool.begin() + size, data);

		}

		//========================================
		// Simple clear of array with 0.0
		//==========================================
		void TzdbHeaderConnector::ClearTransitionPool(const TransitionPool buffer_type)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(rule_pool.begin(), rule_pool.end(), 0.0);
			else
				std::fill(zone_pool.begin(), zone_pool.end(), 0.0);

		}

		//========================================
		// Simple clear of array with 0.0
		//==========================================
		void TzdbHeaderConnector::ClearTransitionPool(const TransitionPool buffer_type, int size)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(rule_pool.begin(), rule_pool.begin() + size, 0.0);
			else
				std::fill(zone_pool.begin(), zone_pool.begin() + size, 0.0);

		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules TzdbHeaderConnector::FindRules(const std::string& name)
		{
			
			auto rule_id = math::GetUniqueID(name);
			return BinarySearchRules(rule_id);
		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules TzdbHeaderConnector::FindRules(uint32_t rule_id)
		{
			
			return BinarySearchRules(rule_id);
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones TzdbHeaderConnector::FindZones(const std::string& name)
		{
			auto zone_id = math::GetUniqueID(name);
			return BinarySearchZones(zone_id);
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones TzdbHeaderConnector::FindZones(uint32_t zone_id)
		{
			
			return BinarySearchZones(zone_id);
		}
	}
}