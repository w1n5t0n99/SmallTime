#include "../include/timezone.h"

#include "../include/basic_datetime.h"
#include "../include/zone_group.h"
#include "../include/rule_group.h"
#include "../include/smalltime_exceptions.h"


namespace smalltime
{
	namespace tz
	{
		//=======================================================
		// Produce UTC offset from a local datetime
		//=======================================================
		RD TimeZone::FixedFromLocal(RD rd, std::string time_zone_name, Choose choose, std::shared_ptr<TzdbConnectorInterface> tzdb_connector)
		{
			auto zoneHandle = tzdb_connector->GetZoneHandle();
			auto zones = tzdb_connector->FindZones(time_zone_name);

			if (zones.first == -1)
				throw InvalidTimeZoneException(time_zone_name);

			// Convert datetime to iso to check with time zones
			BasicDateTime<> iso_dt(rd, KTimeType_Wall);

			ZoneGroup zg(zones, tzdb_connector);
			auto active_zone = zg.FindActiveZone(iso_dt, choose);

			RD total_offset = active_zone->zone_offset;

			if (active_zone->rule_id <= 0)
				return total_offset;

			RuleGroup rg(active_zone->rule_id, active_zone, tzdb_connector);
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
		RD FixedFromUtc(RD rd, std::string time_zone_name, std::shared_ptr<TzdbConnectorInterface> tzdb_connector)
		{
			auto zoneHandle = tzdb_connector->GetZoneHandle();
			auto zones = tzdb_connector->FindZones(time_zone_name);

			if (zones.first == -1)
				throw InvalidTimeZoneException(time_zone_name);

			// Convert datetime to iso to check with time zones
			BasicDateTime<> iso_dt(rd, KTimeType_Utc);

			// converting from utc should not produce an ambig error
			ZoneGroup zg(zones, tzdb_connector);
			auto active_zone = zg.FindActiveZone(iso_dt, Choose::KError);

			RD total_offset = active_zone->zone_offset;

			if (active_zone->rule_id <= 0)
				return total_offset;

			RuleGroup rg(active_zone->rule_id, active_zone, tzdb_connector);
			auto active_rule = rg.FindActiveRule(iso_dt, Choose::KError);

			// No active rule found
			if (!active_rule)
				return total_offset;

			total_offset += active_rule->offset;
			return total_offset;
		}
	}
}