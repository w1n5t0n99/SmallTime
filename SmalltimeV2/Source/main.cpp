#include <iostream>		
#include <vector>
#include <array>

#include "Core\DateTime\LocalDateTime.h"
#include <BasicDateTime.h>
#include <MathUtils.h>
#include <TimeMath.h>
#include "Utils\StlPerfCounter.h"

#include <rule_group.h>
#include "Core\\Tz\tzdb_header_connector.h"
#include <itzdbconnector.h>
#include <memory>
#include <FloatUtil.h>
#include <inttypes.h>

using namespace smalltime;

int main()
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();

	std::shared_ptr<tz::TzdbHeaderConnector> tzdb_connector = std::make_shared<tz::TzdbHeaderConnector>();

	auto rule_handle = tzdb_connector->GetRuleHandle();
	auto rule_id = math::getUniqueID("US");
	auto rules = tzdb_connector->FindRules("US");

	auto zoneHandle = tzdb_connector->GetZoneHandle();
	auto zones = tzdb_connector->FindZones("America/New_York");

	std::cout << "Rules ID: " << rules.rule_id << " Rules first: " << rules.first << " Rules size: " << rules.size << std::endl;
	std::cout << "Zones ID: " << zones.zone_id << " Zones first: " << zones.first << " Zones size: " << zones.size << std::endl;

	tz::RuleGroup ru(rule_id, &zoneHandle[zones.first + zones.size - 1], std::dynamic_pointer_cast<tz::ITzdbConnector, tz::TzdbHeaderConnector>(tzdb_connector));


	BasicDateTime<> dt0(2016, 11, 6, 1, 0, 0, 0, tz::TimeType::KTimeType_Wall);
	BasicDateTime<> dt(2016, 4, 5, 16, 59, 59, 999, tz::TimeType::KTimeType_Utc);

	
	try
	{
		auto ar = ru.FindActiveRule(dt, Choose::Error);
		//std::cout << "Rule offset - " << ar->offset << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	
	
	try
	{
		auto ar = ru.FindActiveRule(dt0, Choose::Error);
		//std::cout << "Rule offset - " << ar->offset << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}


	/*
	if (ar)
	{
		auto roffset = math::hmsFromRd(ar->offset);
		auto zoffset = math::hmsFromRd(zoneHandle[zones.first + zones.size - 1].zoneOffset);
		printf("Active rule found from -  %d  to - %d  in: %d  save - %d:%d:%d:%d\n", ar->fromYear, ar->toYear, ar->month, roffset[0], roffset[1], roffset[2], roffset[3]);
		printf("Zone offset - %d:%d:%d:%d\n\n", zoffset[0], zoffset[1], zoffset[2], zoffset[3]);

		BasicDateTime<> cd(7362238.666666666627862, tz::TimeType_Wall);
		BasicDateTime<> cr(7362238.666666666744277, tz::TimeType_Wall);

		printf("CD ############## - %d/%d/%d %d:%d:%d:%d\n", cd.getYear(), cd.getMonth(), cd.getDay(), cd.getHour(), cd.getMinute(), cd.getSecond(), cd.getMillisecond());
		printf("CR ############## - %d/%d/%d %d:%d:%d:%d\n", cr.getYear(), cr.getMonth(), cr.getDay(), cr.getHour(), cr.getMinute(), cr.getSecond(), cr.getMillisecond());


		Double_t fcd(7362238.666666666627862);
		Double_t fcr(7362238.666666666744277);

		printf("CD ################## -  %" PRIu64 " \n", fcd.i);
		printf("CR ################## -  %" PRIu64 "\n", fcr.i);
		printf("ULP ################## -  %" PRIu64 "\n", fcd.i - fcr.i);

		


	}
	else
	{
		printf("Active rule not found!!\n");
	}
	*/

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	std::cin.get();


	return 0;
}