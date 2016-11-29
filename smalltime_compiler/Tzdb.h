#pragma once
#ifndef _TZDB_
#define _TZDB_
#include "tz_decls.h"
#include <cinttypes>
#include <array>

namespace smalltime
{

namespace tz
{

static const RD KMaxZoneOffset = 0;
static const RD KMinZoneOffset = 0;
static const RD KMaxRuleOffset = 0;
static const int KMaxZoneSize = 1;
static const int KMaxRuleSize = 1;

thread_local static std::array<RD, KMaxZoneSize * 3> zone_pool;
thread_local static std::array<RD, KMaxRuleSize * 3> rule_pool;

static constexpr std::array<Zone,1> KZoneArray = {
Zone {3677025277, 0, -1.1574074074074074e-08, -3651695.0000000116, KTimeType_Wall, 0, 6148613265513525296},

};

static constexpr std::array<Rule,0> KRuleArray = {

};

static constexpr std::array<Zones,1> KZoneLookupArray = {
Zones {3677025277, 0, 1},

};

static constexpr std::array<Rules,0> KRuleLookupArray = {

};

}

}
#endif
