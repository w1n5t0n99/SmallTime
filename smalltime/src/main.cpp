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
	//DateTime<> dt(2016, 12, 10, 2, 0, 0, 0, "Asia/Jerusalem");
	/*
	switch (argc)
	{
	case 10:
		try
		{
			if (strcmp(argv[1], "-julian") == 0)
			{
				smalltime::DateTime<smalltime::chrono::JulianChronology> dt(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), argv[9]);
				smalltime::DateTime<> dt1(dt.GetFixed());
				smalltime::DateTime<smalltime::chrono::IslamicChronology>dt2(dt.GetFixed());
				std::cout << "DateTime  -Julian " << dt << " -Iso " << dt1 << " -Islamic " << dt2 << std::endl;
			}
			else if (strcmp(argv[1], "-iso") == 0)
			{
				smalltime::DateTime<> dt(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), argv[9]);
				std::cout << "DateTime   " << dt << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		break;
	case 9:
		try
		{
			if (strcmp(argv[1], "-julian") == 0)
			{
				smalltime::LocalDateTime<smalltime::chrono::JulianChronology> dt(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]));
				smalltime::LocalDateTime<> dt1(dt.GetFixed());
				std::cout << "LocalDateTime  -Julian " << dt << " -Iso " << dt1 << std::endl;
			}
			else if (strcmp(argv[1], "-iso") == 0)
			{
				smalltime::LocalDateTime<> dt(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8]));
				smalltime::LocalDateTime<smalltime::chrono::HebrewChronology> dt1(dt.GetFixed());
				std::cout << "LocalDateTime   -Iso " << dt <<  "-Hebrew " << dt1 << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		break;
	case 2:
		try
		{
			if (strcmp(argv[1], "-relative-test") == 0)
			{
				smalltime::LocalDateTime<smalltime::chrono::JulianChronology> dt(2016, 12, 27, 5, 50, 0, 0, smalltime::RelSpec::KLastFri);
				smalltime::LocalDateTime<> dt1(2016, 12, 10, 5, 50, 0, 0, smalltime::RelSpec::KLastSun);
				smalltime::LocalDateTime<smalltime::chrono::IslamicChronology>dt2(1438, 3, 10, 5, 50, 0, 0, smalltime::RelSpec::KSunAfter);
				std::cout << "DateTime  -Julian " << dt << " -Iso " << dt1 << " -Islamic " << dt2 << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		break;
	default:
		std::cout << "Error: not correct number of arguments!" << std::endl;
		break;
	}
	*/

	
	
	try
	{
		//smalltime::DateTime<> dt(2016, 3, 13, 1, 23, 59, 59, "America/Adak");
		smalltime::LocalDateTime<> dt(2016, 12, 14, 1, 23, 59, 59);
		smalltime::LocalDateTime<> dt1(dt, smalltime::RS::KMonOnOrBefore);
		std::cout << dt << "  " << dt1 << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	

	/*
	try
	{
		//smalltime::LocalDateTime<> cbt1(2019, 4, 5, 0, 0, 0, 0);
		smalltime::LocalDateTime<smalltime::chrono::HebrewChronology> dt1(5779, 13, 20, 0, 0, 0, 0, smalltime::RS::KLastDay);

		smalltime::LocalDateTime<> iso_fdt(dt1);
		smalltime::LocalDateTime<smalltime::chrono::HebrewChronology> heb_fdt(dt1);
		smalltime::LocalDateTime<smalltime::chrono::IslamicChronology> isl_fdt(dt1);
		smalltime::LocalDateTime<smalltime::chrono::JulianChronology> jul_fdt(dt1);


		std::cout << "LocalDateTime   -Iso " << iso_fdt << "-Hebrew " << heb_fdt << " -Islamic " << isl_fdt << " -Julian " << jul_fdt << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	*/

	/*
	try
	{
		smalltime::LocalDateTime<> local_dt(2016, 12, 13, 22, 0, 0, 0, smalltime::RS::KLastDay);
		smalltime::DateTime<> utc_dt(local_dt, "America/New_York");

		std::cout << "LocalDateTime " << local_dt << "  DateTime " << utc_dt << std::endl; 
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	*/

	/*
	smalltime::BasicDateTime<smalltime::chrono::JulianChronology> jul_fdt(622, 3, 19, 0, 0, 0, 0, smalltime::tz::KTimeType_Wall);
	std::cout << "Persian epoch: " << jul_fdt.GetFixed() << std::endl;

	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;
	*/

	std::cin.get();


	return 0;
}