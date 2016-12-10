#include <iostream>		
#include <vector>
#include <array>

#include "../include/local_datetime.h"
#include <util/stl_perf_counter.h>

#include "../include/local_datetime.h"
#include "../include/datetime.h"

using namespace smalltime;

int main(int argc, char** argv)
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();

	if (argc == 9)
	{
		try
		{
			//DateTime<> dt(2016, 12, 10, 2, 0, 0, 0, "Asia/Jerusalem");
			DateTime<> dt(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), argv[8]);
			std::cout << "DateTime   " << dt << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else if (argc == 8)
	{
		try
		{
			LocalDateTime<> dt(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
			std::cout << "LocalDateTime   " << dt << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else
	{
		std::cout << "Error: not correct number of arguments!" << std::endl;
	}

	

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;

	//std::cin.get();


	return 0;
}