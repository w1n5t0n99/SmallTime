
#include "RuleUtil.h"
#include <TimeMath.h>

namespace smalltime
{
	namespace tz
	{

		//=======================================================
		// Calculate rule transition to the day only
		//=======================================================
		RD RuleUtil::calculateFastTransition(const Rule* rule, int transitionYear, bool withTime)
		{
			HMS hms = { 0, 0, 0, 0 };
			if(withTime)
				hms = math::hmsFromRd(rule->atTime);

			if (rule->dayType == DayType_Dom)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], TimeType_Wall).getRd();
			else if (rule->dayType == DayType_SunGE)
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::SunOnOrAfter, TimeType_Wall).getRd();
			else
				return BasicDateTime<>(transitionYear, rule->month, rule->day, hms[0], hms[1], hms[2], hms[3], RelSpec::LastSun, TimeType_Wall).getRd();
		}

	

	}
}