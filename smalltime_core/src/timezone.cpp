#include "../include/timezone.h"

#include "../include/basic_datetime.h"
#include "../include/zone_group.h"
#include "../include/rule_group.h"
#include "../include/smalltime_exceptions.h"


namespace smalltime
{
	namespace tz
	{
		//==================================
		// Init static member
		//==================================
		TimeZoneDB TimeZone::timezone_db_;

		//=======================================================
		// Produce UTC offset from a local datetime
		//=======================================================
		RD TimeZone::FixedOffsetFromLocal(RD rd, std::string time_zone_name, Choose choose)
		{
			auto zone_handle = timezone_db_.GetZoneHandle();
			auto zones = timezone_db_.FindZones(time_zone_name);

			if (zones.size < 1)
				throw InvalidTimeZoneException(time_zone_name);

			// Convert datetime to iso to check with time zones
			BasicDateTime<> iso_dt(rd, KTimeType_Wall);

			ZoneGroup zg(zones, zone_handle);

			const Zone*  prev_zone = nullptr;
			const Zone*  cur_zone = nullptr;
			std::tie(prev_zone, cur_zone) = zg.FindActiveAndPreviousZone(iso_dt, choose);

			// If iso_dt past DMAX then nullptr is returned and no offset is applied
			// This would be past the year 10,000 so timezones wouldn't be of much use
			RD total_offset = 0.0;
			if (cur_zone)
				total_offset = cur_zone->zone_offset;
			else
				return 0.0;

			if (cur_zone->rule_id <= 0)
				return total_offset;

			// get rule data
			auto rule_arr_ = timezone_db_.GetRuleHandle();
			auto rules = timezone_db_.FindRules(cur_zone->rule_id);
			RuleGroup rg(rules, rule_arr_, cur_zone, prev_zone);

			auto active_rule = rg.FindActiveRule(iso_dt, choose);

			// No active rule found
			if (!active_rule)
				return total_offset;

			total_offset += active_rule->offset;
			return total_offset;
		}

		//=======================================================
		// Produce UTC offset from a local datetime
		//=======================================================
		RD TimeZone::FixedOffsetFromUtc(RD rd, std::string time_zone_name)
		{
			auto zone_handle = timezone_db_.GetZoneHandle();
			auto zones = timezone_db_.FindZones(time_zone_name);

			if (zones.first == -1)
				throw InvalidTimeZoneException(time_zone_name);

			// Convert datetime to iso to check with time zones
			BasicDateTime<> iso_dt(rd, KTimeType_Utc);

			// converting from utc should not produce an ambig error
			ZoneGroup zg(zones, zone_handle);
			
			const Zone*  prev_zone = nullptr;
			const Zone*  cur_zone = nullptr;
			std::tie(prev_zone, cur_zone) = zg.FindActiveAndPreviousZone(iso_dt, Choose::KError);

			// If iso_dt past DMAX then nullptr is returned and no offset is applied
			// This would be past the year 10,000 so timezones wouldn't be of much use
			RD total_offset = 0.0;
			if (cur_zone)
				total_offset = cur_zone->zone_offset;
			else
				return 0.0;

			if (cur_zone->rule_id <= 0)
				return total_offset;

			// get rule data
			auto rule_arr_ = timezone_db_.GetRuleHandle();
			auto rules = timezone_db_.FindRules(cur_zone->rule_id);
			RuleGroup rg(rules, rule_arr_, cur_zone, prev_zone);

			auto active_rule = rg.FindActiveRule(iso_dt, Choose::KError);

			// No active rule found
			if (!active_rule)
				return total_offset;

			total_offset += active_rule->offset;
			return total_offset;
		}


	}
}