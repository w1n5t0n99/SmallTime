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

	const Rule* getRuleHandle();
	const Zone* getZoneHandle();
	
	Rules findRules(const std::string& ruleName);
	Rules findRules(uint32_t ruleId);

	Zones findZones(const std::string& zoneName);
	Zones findZones(uint32_t zoneId);

}
}
#endif
