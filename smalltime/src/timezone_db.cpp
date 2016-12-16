#include "../include/timezone_db.h"
#include <core_math.h>

#include <fstream>

namespace smalltime
{
	namespace tz
	{
		/*
		//===============================================
		// Binary search function for zones
		//===============================================
		Zones BinarySearchZones(uint32_t zone_id, int size)
		{
			int left = 0;
			int right = size - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = KZoneLookupArray[middle];

				if (mid_zones.zone_id == zone_id)
					return mid_zones;
				else if (zone_id > mid_zones.zone_id)
					left = middle + 1;
				else
					right = middle - 1;
			}

			return{ zone_id, -1, -1 };
		}

		//===============================================
		// Binary search function for zones
		//===============================================
		Rules BinarySearchRules(uint32_t rule_id, int size)
		{
			int left = 0;
			int right = size - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = KRuleLookupArray[middle];

				if (mid_zones.rule_id == rule_id)
					return mid_zones;
				else if (rule_id > mid_zones.rule_id)
					left = middle + 1;
				else
					right = middle - 1;
			}

			return{ rule_id, -1, -1 };
		}
		*/

		//=============================================
		// Init tzdb from binary file
		//=============================================
		void TimezoneDB::Init()
		{
			std::ifstream in_file; (path_.c_str(), std::ios::in | std::ios::binary);
			auto tzdb_id = math::GetUniqueID("TZDB_FILE");

			uint32_t in_tzdb_id;
			in_file.read(reinterpret_cast<char*>(&in_tzdb_id), sizeof(in_tzdb_id));

			// check if file id is correct
			if (tzdb_id != in_tzdb_id)
				throw std::runtime_error("tzdb file posibly corrupt, unable to read");

			// check if file length is correct
			in_file.seekg(0, in_file.end);
			size_t file_size = in_file.tellg();
			in_file.seekg(0, in_file.beg);

			size_t in_file_size;
			in_file.read(reinterpret_cast<char*>(&in_file_size), sizeof(in_file_size));

			if(in_file_size != file_size)
				throw std::runtime_error("tzdb file posibly corrupt, unable to read");


		}

	}
}
