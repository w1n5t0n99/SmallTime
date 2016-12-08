#include <iostream>		
#include <vector>
#include <array>

#include "../include/local_datetime.h"
#include <basic_datetime.h>
#include <core_math.h>
#include <time_math.h>
#include <util/stl_perf_counter.h>

#include <rule_group.h>
#include <zone_group.h>
#include "../include/tzdb_header_connector.h"
#include <tzdb_connector_interface.h>
#include <memory>
#include <float_util.h>
#include <inttypes.h>

using namespace smalltime;

int main()
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();

	std::shared_ptr<tz::TzdbHeaderConnector> tzdb_connector = std::make_shared<tz::TzdbHeaderConnector>();

	auto rule_handle = tzdb_connector->GetRuleHandle();
	auto rule_id = math::GetUniqueID("US");
	auto rules = tzdb_connector->FindRules("US");

	auto zoneHandle = tzdb_connector->GetZoneHandle();
	//auto zones = tzdb_connector->FindZones("America/Anchorage");
	auto zones = tzdb_connector->FindZones("America/New_York");

	auto adak_zones = tzdb_connector->FindZones("America/Anchorage");



	std::cout << "Rules ID: " << rules.rule_id << " Rules first: " << rules.first << " Rules size: " << rules.size << std::endl;
	std::cout << "Zones ID: " << zones.zone_id << " Zones first: " << zones.first << " Zones size: " << zones.size << std::endl;

	tz::RuleGroup ru(rule_id, &zoneHandle[zones.first + zones.size - 1], std::dynamic_pointer_cast<tz::TzdbConnectorInterface, tz::TzdbHeaderConnector>(tzdb_connector));

	tz::ZoneGroup zu(zones, std::dynamic_pointer_cast<tz::TzdbConnectorInterface, tz::TzdbHeaderConnector>(tzdb_connector));
	BasicDateTime<> dt(2016, 3, 13, 3, 59, 59, 999, tz::TimeType::KTimeType_Wall);

	tz::ZoneGroup zu1(adak_zones, std::dynamic_pointer_cast<tz::TzdbConnectorInterface, tz::TzdbHeaderConnector>(tzdb_connector));
	BasicDateTime<> dt1(1867, 10, 18, 0, 0, 0, 0, tz::TimeType::KTimeType_Std);



	
	for (int i = adak_zones.first; i < adak_zones.first + adak_zones.size; ++i)
	{
		auto z = zoneHandle[i];
		BasicDateTime<> cr(z.mb_until_utc, z.until_type);
		printf("Zones ############## - %d/%d/%d %d:%d:%d:%d\n", cr.GetYear(), cr.GetMonth(), cr.GetDay(), cr.GetHour(), cr.GetMinute(), cr.GetSecond(), cr.GetMillisecond());
	}

	for (int i = rules.first; i < rules.first + rules.size; ++i)
	{
		auto ar = rule_handle[i];
		printf("Rules From - %d  To - %d  In - %d  On - %d\n", ar.from_year, ar.to_year, ar.month, ar.day);
	}
	
	
	try
	{
		auto ar = ru.FindActiveRule(dt, Choose::KError);
		auto z = zu.FindActiveZone(dt, Choose::KError);
		printf("CLOSEST RULE From - %d  To - %d  In - %d  On - %d\n", ar->from_year, ar->to_year, ar->month, ar->day);
		BasicDateTime<> cr(z->mb_until_utc, z->until_type);
		printf("CLOSEST ZONE - %d/%d/%d %d:%d:%d:%d\n", cr.GetYear(), cr.GetMonth(), cr.GetDay(), cr.GetHour(), cr.GetMinute(), cr.GetSecond(), cr.GetMillisecond());


	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}


	

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	std::cin.get();


	return 0;
}