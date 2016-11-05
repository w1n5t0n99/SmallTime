#include <iostream>		
#include <vector>
#include <array>

#include "Core\DateTime\LocalDateTime.h"
#include <BasicDateTime.h>
#include "Utils\StlPerfCounter.h"
#include "Core\Tz\TzdbAccessor.h"
using namespace smalltime;

int main()
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();
	/*
	auto zoneHandle = tz::getZoneHandle();
	auto zones = tz::findZones("America/New_York");
	std::cout << "Zones ID: " << zones.zoneId << " Zones first: " << zones.first << " Zones size: " << zones.size << std::endl;

	if (zones.size > 0)
	{
		for (int i = zones.first; i < zones.first + zones.size; ++i)
		{
			std::cout << "Zone ID: - " << zoneHandle[i].zoneId << " Until: - " << zoneHandle[i].until << std::endl;
		}
	}
	else
		std::cout << "Zone not found!" << std::endl;
		*/

	BasicDateTime<> dt0(2016, 11, 4, 0, 0, 0, 0, RelSpec::LastFri, tz::TimeType_Wall);

	printf("%d/%d/%d\n", dt0.getYear(), dt0.getMonth(), dt0.getDay());

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	std::cin.get();


	return 0;
}