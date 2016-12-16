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

#include <core_math.h>
#include <fstream>

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

	
	std::ifstream in_file ("tzdb.bin", std::ios::in | std::ios::binary);
	if (!in_file.good())
	{
		std::cout << "Cannot open\n";
	}


	// check if file length is correct
	in_file.seekg(0, in_file.end);
	int file_size = in_file.tellg();
	in_file.seekg(0, in_file.beg);

	auto tzdb_id = smalltime::math::GetUniqueID("TZDB_FILE");

	uint32_t in_tzdb_id;
	in_file.read(reinterpret_cast<char*>(&in_tzdb_id), sizeof(in_tzdb_id));

	// check if file id is correct
	if (tzdb_id != in_tzdb_id)
		throw std::runtime_error("tzdb file posibly corrupt, unable to read");

	std::cout << tzdb_id << "   " << in_tzdb_id << std::endl;

	int in_file_size;

	in_file.read(reinterpret_cast<char*>(&in_file_size), sizeof(in_file_size));

	if (in_file_size != file_size)
		throw std::runtime_error("tzdb file posibly corrupt, unable to read");

	std::cout << file_size << "   " << in_file_size << std::endl;

	in_file.close();


	counter.EndCounter();

	std::cout << "ms elapsed =  " << counter.GetElapsedMilliseconds() << " us elapsed = " << counter.GetElapsedMicroseconds() << std::endl;
	

	std::cin.get();


	return 0;
}