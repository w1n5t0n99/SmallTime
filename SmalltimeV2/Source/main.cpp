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

	auto ruleHandle = tz::getRuleHandle();
	auto rules = tz::findRules("US");

	auto zoneHandle = tz::getZoneHandle();
	auto zones = tz::findZones("America/New_York");

	std::cout << "Zones ID: " << rules.ruleId << " Zones first: " << rules.first << " Zones size: " << rules.size << std::endl;

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

	auto dt = ru.calcWallTransition(&ruleHandle[12], 2016);

	printf("Rule transition at %d/%d/%d %d:%d:%d:%d\n", dt.getYear(), dt.getMonth(), dt.getDay(), dt.getHour(), dt.getMinute(), dt.getSecond(), dt.getMillisecond());

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	std::cin.get();


	return 0;
}