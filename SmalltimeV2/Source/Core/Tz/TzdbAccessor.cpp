#include "TzdbAccessor.h"
#include "Tzdb.h"
#include <MathUtils.h>

namespace smalltime
{
	namespace tz
	{

		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto ZONE_CMP = [](const tz::Zones& lhs, const tz::Zones& rhs)
		{
			return lhs.zoneId < rhs.zoneId;
		};

		//=========================================================
		// Lookup comparer
		//=========================================================
		static auto RULE_CMP = [](const tz::Rules& lhs, const tz::Rules& rhs)
		{
			return lhs.ruleId < rhs.ruleId;
		};
		
		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const Rule* GetRuleHandle()
		{
			return KRuleArray.data();
		}

		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const Zone* GetZoneHandle()
		{
			return KZoneArray.data();
		}

		//===========================================
		// Get pointer allocated array
		//===========================================
		RD* GetTransitionPool(const TransitionPool buffer_type)
		{
			if (buffer_type == TransitionPool::KRule)
				return KRulePool.data();
			else
				return KZonePool.data();
		}

		//========================================
		// Simple fill array with data
		//==========================================
		void FillTransitionPool(const TransitionPool buffer_type, const RD data)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(KRulePool.begin(), KRulePool.end(), data);
			else
				std::fill(KZonePool.begin(), KZonePool.end(), data);

		}

		//========================================
		// Simple fill array with data
		//==========================================
		void FillTransitionPool(const TransitionPool buffer_type, int size, const RD data)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(KRulePool.begin(), KRulePool.begin() + size, data);
			else
				std::fill(KZonePool.begin(), KZonePool.begin() + size, data);

		}

		//========================================
		// Simple clear of array with 0.0
		//==========================================
		void ClearTransitionPool(const TransitionPool buffer_type)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(KRulePool.begin(), KRulePool.end(), 0.0);
			else
				std::fill(KZonePool.begin(), KZonePool.end(), 0.0);

		}

		//========================================
		// Simple clear of array with 0.0
		//==========================================
		void ClearTransitionPool(const TransitionPool buffer_type, int size)
		{
			if (buffer_type == TransitionPool::KRule)
				std::fill(KRulePool.begin(), KRulePool.begin() + size, 0.0);
			else
				std::fill(KZonePool.begin(), KZonePool.begin() + size , 0.0);

		}

		//===========================================
		// Return max size
		//===========================================
		int GetMaxRuleSize()
		{
			return KMaxRuleSize;
		}

		//===========================================
		// Return max size
		//===========================================
		int GetMaxZoneSize()
		{
			return KMaxZoneSize;
		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules FindRules(const std::string& name)
		{
			auto ruleId = math::getUniqueID(name);
			Rules searchRules = { ruleId, 0, 0 };

			const auto& foundIt = std::lower_bound(KRuleLookupArray.begin(), KRuleLookupArray.end(), searchRules, RULE_CMP);

			if (foundIt != KRuleLookupArray.end())
				return *foundIt;
			else
				return { ruleId, -1, -1 };
		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules FindRules(uint32_t ruleId)
		{
			Rules searchRules = { ruleId, 0, 0 };

			const auto& foundIt = std::lower_bound(KRuleLookupArray.begin(), KRuleLookupArray.end(), searchRules, RULE_CMP);

			if (foundIt != KRuleLookupArray.end())
				return *foundIt;
			else
				return{ ruleId, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones FindZones(const std::string& name)
		{
			auto zoneId = math::getUniqueID(name);
			Zones searchZones = { zoneId, 0, 0 };

			const auto& foundIt = std::lower_bound(KZoneLookupArray.begin(), KZoneLookupArray.end(), searchZones, ZONE_CMP);

			if (foundIt != KZoneLookupArray.end())
				return *foundIt;
			else
				return{ zoneId, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones FindZones(uint32_t zoneId)
		{
			Zones searchZones = { zoneId, 0, 0 };

			const auto& foundIt = std::lower_bound(KZoneLookupArray.begin(), KZoneLookupArray.end(), searchZones, ZONE_CMP);

			if (foundIt != KZoneLookupArray.end())
				return *foundIt;
			else
				return{ zoneId, -1, -1 };
		}


	}
}