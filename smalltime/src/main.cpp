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
#include "../include/datetime.h"
#include <murmur_hash3.h>

using namespace smalltime;

int main()
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();

	try
	{
		DateTime<> dt(2016, 4, 3, 3, 0, 0, 0, "Australia/Hobart");
		std::cout << "DT   " << dt << std::endl;
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