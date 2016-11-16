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
		// Get pointer allocated buffer
		//===========================================
		RD* GetRDBuffer(const RDBufferType buffer_type)
		{
			if (buffer_type == RDBufferType::KPrimaryRule)
				return KPrimaryRuleRdBuffer.data();
			else if (buffer_type == RDBufferType::KSecondaryRule)
				return KSecondaryRuleRdBuffer.data();
			else
				return KZoneRdBuffer.data();
		}

		//========================================
		// Simple fill buffer array with data
		//==========================================
		void FillRDBuffer(const RDBufferType buffer_type, const RD data)
		{
			if (buffer_type == RDBufferType::KPrimaryRule)
				std::fill(KPrimaryRuleRdBuffer.begin(), KPrimaryRuleRdBuffer.end(), data);
			else if (buffer_type == RDBufferType::KSecondaryRule)
				std::fill(KSecondaryRuleRdBuffer.begin(), KSecondaryRuleRdBuffer.end(), data);
			else
				std::fill(KZoneRdBuffer.begin(), KZoneRdBuffer.end(), data);

		}

		//===========================================
		// Get pointer allocated buffer
		//===========================================
		int* GetIntBuffer(const IntBufferType buffer_type)
		{
			if (buffer_type == IntBufferType::KRule)
				return KRuleIntBuffer.data();
			else
				return nullptr;
		}

		//========================================
		// Simple fill buffer array with data
		//==========================================
		void FillIntBuffer(const IntBufferType buffer_type, const int data)
		{
			if (buffer_type == IntBufferType::KRule)
				std::fill(KRuleIntBuffer.begin(), KRuleIntBuffer.end(), data);
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