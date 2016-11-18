#pragma once
#ifndef _TZDBACCESSOR_
#define _TZDBACCESSOR_

#include <CoreDecls.h>
#include <TzDecls.h>

#include <cinttypes>
#include <string>

namespace smalltime
{
namespace tz
{

	const Rule* GetRuleHandle();
	const Zone* GetZoneHandle();

	enum class TransitionPool
	{
		KPrimaryRule,
		KSecondaryRule,
		KPrimaryZone,
		KSecondaryZone
	};

	RD* GetTransitionPool(const TransitionPool buffer_type);
	void FillTransitionPool(const TransitionPool buffer_type, const RD data);
	void ClearTransitionPool(const TransitionPool buffer_type);

	int GetMaxRuleSize();
	int GetMaxZoneSize();
	
	Rules FindRules(const std::string& ruleName);
	Rules FindRules(uint32_t ruleId);

	Zones FindZones(const std::string& zoneName);
	Zones FindZones(uint32_t zoneId);

}
}
#endif
