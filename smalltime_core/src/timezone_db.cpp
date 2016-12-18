#include "../include/timezone_db.h"
#include "../include/core_math.h"
#include "../include/file_util.h"

#include <fstream>

namespace smalltime
{
	namespace tz
	{

		//==================================
		// Init static member
		//==================================
		std::unique_ptr<Zone[]> TimeZoneDB::zone_arr_(nullptr);
		std::unique_ptr<Rule[]> TimeZoneDB::rule_arr_(nullptr);
		std::unique_ptr<Zones[]> TimeZoneDB::zone_lookup_arr_(nullptr);
		std::unique_ptr<Rules[]> TimeZoneDB::rule_lookup_arr_(nullptr);
	
		std::string TimeZoneDB::path_ = "tzdb.bin";
		int TimeZoneDB::zone_size_ = 0;
		int TimeZoneDB::rule_size_ = 0;
		int TimeZoneDB::zone_lookup_size_ = 0;
		int TimeZoneDB::rule_lookup_size_ = 0;
		bool TimeZoneDB::initialized_ = false;

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

		
		//===============================================
		// Binary search function for zones
		//===============================================
		Zones TimeZoneDB::BinarySearchZones(uint32_t zone_id, int size)
		{
			int left = 0;
			int right = size - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = zone_lookup_arr_[middle];

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
		Rules TimeZoneDB::BinarySearchRules(uint32_t rule_id, int size)
		{
			int left = 0;
			int right = size - 1;

			while (left <= right)
			{
				int middle = (left + right) / 2;
				const auto& mid_zones = rule_lookup_arr_[middle];

				if (mid_zones.rule_id == rule_id)
					return mid_zones;
				else if (rule_id > mid_zones.rule_id)
					left = middle + 1;
				else
					right = middle - 1;
			}

			return{ rule_id, -1, -1 };
		}

		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const Rule* const TimeZoneDB::GetRuleHandle()
		{
			if (!initialized_)
				Init();

			return rule_arr_.get();
		}

		//===============================================
		// Get pointer to first element of tzdb array
		//================================================
		const Zone* const TimeZoneDB::GetZoneHandle()
		{
			if (!initialized_)
				Init();

			return zone_arr_.get();
		}
		
		//================================================
		// Find rules matching name id
		//================================================
		Rules TimeZoneDB::FindRules(const std::string& name)
		{
			if (!initialized_)
				Init();

			auto rule_id = math::GetUniqueID(name);
			return BinarySearchRules(rule_id, rule_lookup_size_);
		}

		//================================================
		// Find rules matching name id
		//================================================
		Rules TimeZoneDB::FindRules(uint32_t rule_id)
		{
			if (!initialized_)
				Init();

			return BinarySearchRules(rule_id, rule_lookup_size_);
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones TimeZoneDB::FindZones(const std::string& name)
		{
			if (!initialized_)
				Init();

			auto zone_id = math::GetUniqueID(name);
			return BinarySearchZones(zone_id, zone_lookup_size_);
		}

		//================================================
		// Find zones matching name id
		//================================================
		Zones TimeZoneDB::FindZones(uint32_t zone_id)
		{
			if (!initialized_)
				Init();

			return BinarySearchZones(zone_id, zone_lookup_size_);
		}

		//=============================================
		// Init tzdb from binary file
		//=============================================
		void TimeZoneDB::Init()
		{
			if (initialized_)
				return;

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

			int in_file_size = 0;
			in_file.read(reinterpret_cast<char*>(&in_file_size), sizeof(in_file_size));

			if (in_file_size != file_size)
				throw std::runtime_error("tzdb file posibly corrupt, unable to read");

			zone_size_ = 0;
			in_file.read(reinterpret_cast<char*>(&zone_size_), sizeof(zone_size_));
			zone_size_ /= KZONE_SIZE;

			// init and populate zone array
			zone_arr_ = std::unique_ptr<Zone[]>{ new Zone[zone_size_] };
			for (int i = 0; i < zone_size_; ++i)
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

			int rule_size_ = 0;
			in_file.read(reinterpret_cast<char*>(&rule_size_), sizeof(rule_size_));
			rule_size_ /= KRULE_SIZE;

			// init and populate rule array
			rule_arr_ = std::unique_ptr<Rule[]>{ new Rule[rule_size_] };
			for (int i = 0; i < rule_size_; ++i)
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

			zone_lookup_size_ = 0;
			in_file.read(reinterpret_cast<char*>(&zone_lookup_size_), sizeof(zone_lookup_size_));
			zone_lookup_size_ /= KZONES_SIZE;

			// init and populate zone lookup array
			zone_lookup_arr_ = std::unique_ptr<Zones[]>{ new Zones[zone_lookup_size_] };
			for (int i = 0; i < zone_lookup_size_; ++i)
			{
				in_file.read(reinterpret_cast<char*>(&zone_lookup_arr_[i].zone_id), sizeof(zone_lookup_arr_[i].zone_id));
				in_file.read(reinterpret_cast<char*>(&zone_lookup_arr_[i].first), sizeof(zone_lookup_arr_[i].first));
				in_file.read(reinterpret_cast<char*>(&zone_lookup_arr_[i].size), sizeof(zone_lookup_arr_[i].size));
			}


			rule_lookup_size_ = 0;
			in_file.read(reinterpret_cast<char*>(&rule_lookup_size_), sizeof(rule_lookup_size_));
			rule_lookup_size_ /= KRULES_SIZE;

			// init and populate rule lookup array
			rule_lookup_arr_ = std::unique_ptr<Rules[]>{ new Rules[rule_lookup_size_] };
			for (int i = 0; i < rule_lookup_size_; ++i)
			{
				in_file.read(reinterpret_cast<char*>(&rule_lookup_arr_[i].rule_id), sizeof(rule_lookup_arr_[i].rule_id));
				in_file.read(reinterpret_cast<char*>(&rule_lookup_arr_[i].first), sizeof(rule_lookup_arr_[i].first));
				in_file.read(reinterpret_cast<char*>(&rule_lookup_arr_[i].size), sizeof(rule_lookup_arr_[i].size));
			}

			initialized_ = true;
		}

		//========================================
		// Set path to look for tzdb file
		//========================================
		void TimeZoneDB::SetPath(std::string path)
		{
			path_ = std::move(path);
		//	path_ = fileutil::ExtractParent(path);
		//	path_ = fileutil::AddSeparator(path_);
			path_ += "tzdb.bin";

			initialized_ = false;

		}

	}
}
