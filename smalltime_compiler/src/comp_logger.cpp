#include "..\include\comp_logger.h"
#include <string>
#include <iomanip>
#include <assert.h>

#include <time_math.h>
#include <basic_datetime.h>


namespace smalltime
{
	namespace comp
	{
		//=============================================
		// Log all zone data
		//=============================================
		void CompLogger::LogAllZones(std::ostream& stream, std::vector<tz::Zone>& vec_zone, std::vector<ZoneData>& vec_zonedata)
		{
			stream << std::setprecision(17);
			//UTC zone added as last element in vec_zone
			assert(vec_zone.size() == vec_zonedata.size() + 1);
			for (int i = 0; i < vec_zone.size(); ++i)
			{
				auto zone_offset = math::HmsFromFixed(vec_zone[i].zone_offset);
				auto zone_diff = vec_zone[i].until_diff;

				BasicDateTime<> wall_dt(vec_zone[i].until_wall, tz::KTimeType_Wall);
				BasicDateTime<> until_dt(vec_zone[i].until_utc, tz::KTimeType_Utc);

				std::string zone_name = "";
				if (i == vec_zone.size() - 1)
					zone_name = "UTC";
				else
					zone_name = vec_zonedata[i].name;

				stream << zone_name << " - " << vec_zone[i].zone_id << " Wall Until - " << wall_dt.GetYear() << "/" << wall_dt.GetMonth() << "/" << wall_dt.GetDay() << " " << wall_dt.GetHour() << ":"
					<< wall_dt.GetMinute() << ":" << wall_dt.GetSecond() << ":" << wall_dt.GetMillisecond() << " Until Diff - " << zone_diff << "\n";

			}
		}

	}
}