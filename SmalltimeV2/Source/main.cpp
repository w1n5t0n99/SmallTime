#include <iostream>		
#include <vector>
#include <array>

#include "Core\DateTime\LocalDateTime.h"
#include <BasicDateTime.h>
#include <MathUtils.h>
#include <TimeMath.h>
#include "Utils\StlPerfCounter.h"
#include "Core\Tz\TzdbAccessor.h"
#include "Core\Tz\RuleUtil.h"
using namespace smalltime;

int main()
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();

	auto rule_handle = tz::GetRuleHandle();
	auto rule_id = math::getUniqueID("AT");
	auto rules = tz::FindRules("AT");

	auto zoneHandle = tz::GetZoneHandle();
	auto zones = tz::FindZones("Australia/Hobart");

	std::cout << "Rules ID: " << rules.ruleId << " Rules first: " << rules.first << " Rules size: " << rules.size << std::endl;
	std::cout << "Zones ID: " << zones.zoneId << " Zones first: " << zones.first << " Zones size: " << zones.size << std::endl;

	tz::RuleUtil ru(rule_id, &zoneHandle[zones.first + zones.size - 1]);

	//auto dt1 = ru.CalcTransitionWall(&rule_handle[rules.first + rules.size - 1], 2016);
	//counter.StartCounter();

	/*
	int ri = 0;
	auto pr = ru.findPreviousRule(&ruleHandle[ri], 1918);
	
	if (pr)
	{
		std::cout << "Current rule From: " << ruleHandle[ri].fromYear << " To: " << ruleHandle[ri].toYear << " In: " << ruleHandle[ri].month << std::endl;
		std::cout << "Prev rule found! From: " << pr->fromYear << " To: " << pr->toYear << " In: " << pr->month << std::endl;
	}
	else
		std::cout << "Prev rule not found!" << std::endl;

		*/

	//int next_closest_year = ru.FindNextActiveYear(2005);
	//std::cout << "Next active year = " << next_closest_year << std::endl;

	//auto dt = ru.CalcTransitionWall(&rule_handle[rules.first + rules.size - 1], 2016);

	//printf("Rule transition at %d/%d/%d %d:%d:%d:%d\n", dt.getYear(), dt.getMonth(), dt.getDay(), dt.getHour(), dt.getMinute(), dt.getSecond(), dt.getMillisecond());

	BasicDateTime<> dt(2016, 11, 22, 9, 15, 0, 0, tz::TimeType::TimeType_Wall);
//	BasicDateTime<> dt(2016, 4, 3, 3, 0, 0, 0, tz::TimeType::TimeType_Wall);
	auto ar = ru.FindActiveRule(dt, Choose::Error);

	if (ar)
	{
		auto roffset = math::hmsFromRd(ar->offset);
		auto zoffset = math::hmsFromRd(zoneHandle[zones.first + zones.size - 1].zoneOffset);
		printf("Active rule found from -  %d  to - %d  in: %d  save - %d:%d:%d:%d\n", ar->fromYear, ar->toYear, ar->month, roffset[0], roffset[1], roffset[2], roffset[3]);
		printf("Zone offset - %d:%d:%d:%d\n\n", zoffset[0], zoffset[1], zoffset[2], zoffset[3]);

		BasicDateTime<> dtw(2016, 11, 22, 9, 15, 0, 0, tz::TimeType::TimeType_Wall);
		printf("DateTime wall time - %d/%d/%d %d:%d:%d:%d\n", dtw.getYear(), dtw.getMonth(), dtw.getDay(), dtw.getHour(), dtw.getMinute(), dtw.getSecond(), dtw.getMillisecond());
		auto rdu = dtw.getRd() - zoneHandle[zones.first + zones.size - 1].zoneOffset - ar->offset;
		BasicDateTime<> dtu(rdu, tz::TimeType_Utc);
		printf("DateTime utc time - %d/%d/%d %d:%d:%d:%d\n", dtu.getYear(), dtu.getMonth(), dtu.getDay(), dtu.getHour(), dtu.getMinute(), dtu.getSecond(), dtu.getMillisecond());


	}
	else
	{
		printf("Active rule not found!!\n");
	}

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	std::cin.get();


	return 0;
}