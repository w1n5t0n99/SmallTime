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
	std::cout << "Zones ID: " << rules.ruleId << " Zones first: " << rules.first << " Zones size: " << rules.size << std::endl;

	tz::RuleGroup rg = { ruleHandle, rules.first, rules.size };
	tz::RuleUtil ru(rg, nullptr);

	auto pr = ru.findPreviousRule(&ruleHandle[11], 2012);
	
	if (pr)
	{
		std::cout << "Prev rule found! From: " << pr->fromYear << " To: " << pr->toYear << " In: " << pr->month << std::endl;
	}
	else
		std::cout << "Prev rule not found!" << std::endl;

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	std::cin.get();


	return 0;
}