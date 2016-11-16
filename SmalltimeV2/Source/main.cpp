#include <iostream>		
#include <vector>
#include <array>

#include "Core\DateTime\LocalDateTime.h"
#include <BasicDateTime.h>
#include "Utils\StlPerfCounter.h"
#include "Core\Tz\TzdbAccessor.h"
#include "Core\Tz\RuleUtil.h"
using namespace smalltime;

int main()
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();

	auto ruleHandle = tz::GetRuleHandle();
	auto rules = tz::FindRules("AT");

	auto zoneHandle = tz::GetZoneHandle();
	auto zones = tz::FindZones("Australia/Hobart");

	std::cout << "Rules ID: " << rules.ruleId << " Rules first: " << rules.first << " Rules size: " << rules.size << std::endl;
	std::cout << "Zones ID: " << zones.zoneId << " Zones first: " << zones.first << " Zones size: " << zones.size << std::endl;


	tz::RuleGroup rg = { ruleHandle, rules.first, rules.size };
	tz::RuleUtil ru(rg, &zoneHandle[zones.first + zones.size - 1]);

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

	auto dt = ru.calcWallTransition(&ruleHandle[rules.first + rules.size - 1], 2016);

	printf("Rule transition at %d/%d/%d %d:%d:%d:%d\n", dt.getYear(), dt.getMonth(), dt.getDay(), dt.getHour(), dt.getMinute(), dt.getSecond(), dt.getMillisecond());

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	std::cin.get();


	return 0;
}