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

	enum class RDBufferType
	{
		KPrimaryRule,
		KSecondaryRule,
		KZone
	};

	enum class IntBufferType
	{
		KRule,
		KZone
	};

	RD* GetRDBuffer(const RDBufferType buffer_type);
	void FillRDBuffer(const RDBufferType buffer_type, const RD data);

	int* GetIntBuffer(const IntBufferType buffer_type);
	void FillIntBuffer(const IntBufferType buffer_type, const int data);
	
	Rules FindRules(const std::string& ruleName);
	Rules FindRules(uint32_t ruleId);

	Zones FindZones(const std::string& zoneName);
	Zones FindZones(uint32_t zoneId);

}
}
#endif
