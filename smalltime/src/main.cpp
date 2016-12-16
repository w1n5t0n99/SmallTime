#include <iostream>		
#include <vector>
#include <array>

#include "../include/local_datetime.h"
#include <util/stl_perf_counter.h>

#include <basic_datetime.h>
#include "../include/local_datetime.h"
#include "../include/datetime.h"

#include "../include/julian_chronology.h"
#include "../include/islamic_chronology.h"
#include "../include/hebrew_chronology.h"

int main(int argc, char** argv)
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();
	
	
	try
	{
		smalltime::DateTime<> dt(2016, 12, 15, 1, 51, 0, 0, "America/New_York");
		//smalltime::LocalDateTime<> dt(2016, 12, 14, 3, 12, 0, 0);
		smalltime::LocalDateTime<> dt1(dt, "Pacific/Honolulu");

		//smalltime::DateTime<> dt1(dt, "America/New_York");
		smalltime::DateTime<smalltime::chrono::JulianChronology>dt2(dt);
		std::cout << dt << "  " << dt1 << "  " << dt2 << std::endl;

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