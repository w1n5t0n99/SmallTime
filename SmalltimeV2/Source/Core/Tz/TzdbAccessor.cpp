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
		const Rule* getRuleHandle()
		{
			return RULE_ARRAY.data();
		}


		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const Zone* getZoneHandle()
		{
			return ZONE_ARRAY.data();
		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules findRules(const std::string& name)
		{
			auto ruleId = math::getUniqueID(name);
			Rules searchRules = { ruleId, 0, 0 };

			const auto& foundIt = std::lower_bound(RULE_LOOKUP_ARRAY.begin(), RULE_LOOKUP_ARRAY.end(), searchRules, RULE_CMP);

			if (foundIt != RULE_LOOKUP_ARRAY.end())
				return *foundIt;
			else
				return { ruleId, -1, -1 };
		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules findRules(uint32_t ruleId)
		{
			Rules searchRules = { ruleId, 0, 0 };

			const auto& foundIt = std::lower_bound(RULE_LOOKUP_ARRAY.begin(), RULE_LOOKUP_ARRAY.end(), searchRules, RULE_CMP);

			if (foundIt != RULE_LOOKUP_ARRAY.end())
				return *foundIt;
			else
				return{ ruleId, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones findZones(const std::string& name)
		{
			auto zoneId = math::getUniqueID(name);
			Zones searchZones = { zoneId, 0, 0 };

			const auto& foundIt = std::lower_bound(ZONE_LOOKUP_ARRAY.begin(), ZONE_LOOKUP_ARRAY.end(), searchZones, ZONE_CMP);

			if (foundIt != ZONE_LOOKUP_ARRAY.end())
				return *foundIt;
			else
				return{ zoneId, -1, -1 };
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones findZones(uint32_t zoneId)
		{
			Zones searchZones = { zoneId, 0, 0 };

			const auto& foundIt = std::lower_bound(ZONE_LOOKUP_ARRAY.begin(), ZONE_LOOKUP_ARRAY.end(), searchZones, ZONE_CMP);

			if (foundIt != ZONE_LOOKUP_ARRAY.end())
				return *foundIt;
			else
				return{ zoneId, -1, -1 };
		}

	}
}