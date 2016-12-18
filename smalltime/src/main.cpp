#include <iostream>		
#include <vector>
#include <array>

#include <util/stl_perf_counter.h>

#include "../include/local_datetime.h"
#include "../include/datetime.h"

#include "../include/julian_chronology.h"
#include "../include/islamic_chronology.h"
#include "../include/hebrew_chronology.h"




int main(int argc, char** argv)
{
	StlPerfCounter counter("Counter");
	counter.StartCounter();
	
	//smalltime::SetTimeZoneFilePath("C:\\Users\\reede\\Documents\\");
	
	if (argc <= 1)
		return 0;

 
	if (strcmp(argv[1], "-datetime") == 0)
	{

		try
		{
			smalltime::DateTime<> dt(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), argv[9]);
			std::cout << dt << " utc " << std::endl;

		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else if (strcmp(argv[1], "-localdatetime") == 0)
	{
		try
		{
			smalltime::LocalDateTime<> dt(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]));
			std::cout << dt << " local " << std::endl;


		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else if (strcmp(argv[1], "-convert") == 0 && strcmp(argv[9], "-from") == 0 && strcmp(argv[11], "-to") == 0)
	{
		smalltime::LocalDateTime<> dt(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]));
		// must be a conversion -from(9) "timezone"(10) -to(11) "timezone"(12)
		smalltime::DateTime<> utc_dt(dt, argv[10]);
		smalltime::LocalDateTime<> local_dt(utc_dt, argv[12]);

		std::cout << dt << " " << argv[10] << " " << local_dt << " " << argv[12] << std::endl;
	}

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;
	

	//std::cin.get();


	return 0;
}