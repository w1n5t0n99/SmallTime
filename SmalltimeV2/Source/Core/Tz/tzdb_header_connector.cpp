#include "tzdb_header_connector.h"
#include "Tzdb.h"
#include <core_math.h>

namespace smalltime
{
	namespace tz
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
			Rules searchRules = { rule_id, 0, 0 };

			const auto& foundIt = std::lower_bound(KRuleLookupArray.begin(), KRuleLookupArray.end(), searchRules, RULE_CMP);

			if (foundIt != KRuleLookupArray.end())
				return *foundIt;
			else
				return{ rule_id, -1, -1 };
		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules TzdbHeaderConnector::FindRules(uint32_t rule_id)
		{
			Rules searchRules = { rule_id, 0, 0 };

			const auto& foundIt = std::lower_bound(KRuleLookupArray.begin(), KRuleLookupArray.end(), searchRules, RULE_CMP);

			if (foundIt != KRuleLookupArray.end())
				return *foundIt;
			else
				return{ rule_id, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones TzdbHeaderConnector::FindZones(const std::string& name)
		{
			auto zone_id = math::GetUniqueID(name);
			Zones searchZones = { zone_id, 0, 0 };

			const auto& foundIt = std::lower_bound(KZoneLookupArray.begin(), KZoneLookupArray.end(), searchZones, ZONE_CMP);

			if (foundIt != KZoneLookupArray.end())
				return *foundIt;
			else
				return{ zone_id, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones TzdbHeaderConnector::FindZones(uint32_t zone_id)
		{
			Zones searchZones = { zone_id, 0, 0 };

			const auto& foundIt = std::lower_bound(KZoneLookupArray.begin(), KZoneLookupArray.end(), searchZones, ZONE_CMP);

			if (foundIt != KZoneLookupArray.end())
				return *foundIt;
			else
				return{ zone_id, -1, -1 };
		}
	}
}