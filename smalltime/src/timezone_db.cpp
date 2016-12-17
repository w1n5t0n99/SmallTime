#include "../include/timezone_db.h"
#include <core_math.h>
#include <file_util.h>

#include <fstream>

namespace smalltime
{
	namespace tz
	{

		//==================================
		// Init static member
		//==================================
		std::unique_ptr<Zone[]> TimezoneDB::zone_arr_(nullptr);
		std::unique_ptr<Rule[]> TimezoneDB::rule_arr_(nullptr);
		std::unique_ptr<Zones[]> TimezoneDB::zone_lookup_arr_(nullptr);
		std::unique_ptr<Rules[]> TimezoneDB::rule_lookup_arr_(nullptr);

		static constexpr tz::Zone KZONE;
		static constexpr int KZONE_SIZE = sizeof(KZONE.abbrev) + sizeof(KZONE.mb_rule_offset) + sizeof(KZONE.mb_until_utc) + sizeof(KZONE.next_zone_offset) +
			sizeof(KZONE.rule_id) + sizeof(KZONE.trans_rule_offset) + sizeof(KZONE.until_type) + sizeof(KZONE.zone_id) + sizeof(KZONE.zone_offset);

		static constexpr tz::Rule KRULE;
		static constexpr int KRULE_SIZE = sizeof(KRULE.at_time) + sizeof(KRULE.at_type) + sizeof(KRULE.day) + sizeof(KRULE.day_type) +
			sizeof(KRULE.from_year) + sizeof(KRULE.letter) + sizeof(KRULE.month) + sizeof(KRULE.offset) + sizeof(KRULE.rule_id) + sizeof(KRULE.to_year);

		static constexpr tz::Zones KZONES;
		static constexpr int KZONES_SIZE = sizeof(KZONES.first) + sizeof(KZONES.size) + sizeof(KZONES.zone_id);


		static constexpr tz::Zones KRULES;
		static constexpr int KRULES_SIZE = sizeof(KRULES.first) + sizeof(KRULES.size) + sizeof(KRULES.zone_id);

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
			std::ifstream in_file (path_.c_str(), std::ios::in | std::ios::binary);
			auto tzdb_id = math::GetUniqueID("TZDB_FILE");

			// check if file length is correct
			in_file.seekg(0, in_file.end);
			int file_size = in_file.tellg();
			in_file.seekg(0, in_file.beg);

			uint32_t in_tzdb_id = 0;
			in_file.read(reinterpret_cast<char*>(&in_tzdb_id), sizeof(in_tzdb_id));

			// check if file id is correct
			if (tzdb_id != in_tzdb_id)
				throw std::runtime_error("tzdb file posibly corrupt, unable to read");

			std::cout << tzdb_id << "   " << in_tzdb_id << std::endl;

			int in_file_size = 0;
			in_file.read(reinterpret_cast<char*>(&in_file_size), sizeof(in_file_size));

			if (in_file_size != file_size)
				throw std::runtime_error("tzdb file posibly corrupt, unable to read");

			int zone_arr_size = 0;
			in_file.read(reinterpret_cast<char*>(&zone_arr_size), sizeof(zone_arr_size));
			zone_arr_size /= KZONE_SIZE;

			// init and populate zone array
			zone_arr_ = std::unique_ptr<Zone[]>{ new Zone[zone_arr_size] };
			for (int i = 0; i < zone_arr_size; ++i)
			{
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].zone_id), sizeof(zone_arr_[i].zone_id));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].rule_id), sizeof(zone_arr_[i].rule_id));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].mb_until_utc), sizeof(zone_arr_[i].mb_until_utc));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].until_type), sizeof(zone_arr_[i].until_type));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].zone_offset), sizeof(zone_arr_[i].zone_offset));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].next_zone_offset), sizeof(zone_arr_[i].next_zone_offset));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].mb_rule_offset), sizeof(zone_arr_[i].mb_rule_offset));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].trans_rule_offset), sizeof(zone_arr_[i].trans_rule_offset));
				in_file.read(reinterpret_cast<char*>(&zone_arr_[i].abbrev), sizeof(zone_arr_[i].abbrev));
			}

			int rule_arr_size = 0;
			in_file.read(reinterpret_cast<char*>(&rule_arr_size), sizeof(rule_arr_size));
			rule_arr_size /= KRULE_SIZE;

			// init and populate rule array
			rule_arr_ = std::unique_ptr<Rule[]>{ new Rule[rule_arr_size] };
			for (int i = 0; i < rule_arr_size; ++i)
			{
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].rule_id), sizeof(rule_arr_[i].rule_id));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].from_year), sizeof(rule_arr_[i].from_year));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].to_year), sizeof(rule_arr_[i].to_year));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].month), sizeof(rule_arr_[i].month));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].day), sizeof(rule_arr_[i].day));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].day_type), sizeof(rule_arr_[i].day_type));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].at_time), sizeof(rule_arr_[i].at_time));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].at_type), sizeof(rule_arr_[i].at_type));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].offset), sizeof(rule_arr_[i].offset));
				in_file.read(reinterpret_cast<char*>(&rule_arr_[i].letter), sizeof(rule_arr_[i].letter));

			}

			int zone_lookup_size = 0;
			in_file.read(reinterpret_cast<char*>(&zone_lookup_size), sizeof(zone_lookup_size));
			zone_lookup_size /= KZONES_SIZE;

			// init and populate zone lookup array
			zone_lookup_arr_ = std::unique_ptr<Zones[]>{ new Zones[zone_lookup_size] };
			for (int i = 0; i < zone_lookup_size; ++i)
			{
				in_file.read(reinterpret_cast<char*>(&zone_lookup_arr_[i].zone_id), sizeof(zone_lookup_arr_[i].zone_id));
				in_file.read(reinterpret_cast<char*>(&zone_lookup_arr_[i].first), sizeof(zone_lookup_arr_[i].first));
				in_file.read(reinterpret_cast<char*>(&zone_lookup_arr_[i].size), sizeof(zone_lookup_arr_[i].size));
			}


			int rule_lookup_size = 0;
			in_file.read(reinterpret_cast<char*>(&rule_lookup_size), sizeof(rule_lookup_size));
			rule_lookup_size /= KRULES_SIZE;

			// init and populate rule lookup array
			rule_lookup_arr_ = std::unique_ptr<Rules[]>{ new Rules[rule_lookup_size] };
			for (int i = 0; i < zone_lookup_size; ++i)
			{
				in_file.read(reinterpret_cast<char*>(&rule_lookup_arr_[i].rule_id), sizeof(rule_lookup_arr_[i].rule_id));
				in_file.read(reinterpret_cast<char*>(&rule_lookup_arr_[i].first), sizeof(rule_lookup_arr_[i].first));
				in_file.read(reinterpret_cast<char*>(&rule_lookup_arr_[i].size), sizeof(rule_lookup_arr_[i].size));
			}


		}

		//========================================
		// Set path to look for tzdb file
		//========================================
		void TimezoneDB::SetPath(std::string path)
		{
			path_ = fileutil::StripSeparator(fileutil::StripExt(path));
			path_ = fileutil::AddSeparator(path_);
			path_ += "tzdb.bin";
		}

	}
}
