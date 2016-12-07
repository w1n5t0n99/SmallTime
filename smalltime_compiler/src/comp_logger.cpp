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
				auto zone_diff = vec_zone[i].mb_rule_offset;

				tz::ZoneTransition zt(vec_zone[i].mb_until_utc, vec_zone[i].zone_offset, vec_zone[i].next_zone_offset, vec_zone[i].mb_rule_offset, vec_zone[i].trans_rule_offset);

				BasicDateTime<> mb_wall_dt(zt.mb_trans_wall_, tz::KTimeType_Wall);
				BasicDateTime<> fi_wall_dt(zt.first_inst_wall_, tz::KTimeType_Wall);
				BasicDateTime<> until_wall_dt(zt.trans_wall_, tz::KTimeType_Wall);


				std::string zone_name = "";
				if (i == vec_zone.size() - 1)
					zone_name = "UTC";
				else
					zone_name = vec_zonedata[i].name;

				stream << zone_name << " Moment Before " << mb_wall_dt << " First Instant " << fi_wall_dt << " Until Wall " << until_wall_dt << std::endl;

			}
		}

	}
}