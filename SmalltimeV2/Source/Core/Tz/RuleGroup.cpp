#include "RuleGroup.h"
#include <TimeMath.h>

namespace smalltime
{
	namespace tz
	{

		//=======================================================
		// Calculate rule transition to the day only
		//=======================================================
		RD RuleGroup::calculateFastTransition(const Rule* rule, int transitionYear, bool withTime)
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

		//============================================================
		// Calculate rule transition with exact time and time type
		//=============================================================
		RD RuleGroup::calculateExactTransition(const Rule* rule, const Zone* zone, int transitionYear, TimeType tmType)
		{
			if (rule->atType == tmType)
				return calculateFastTransition(rule, transitionYear, true);




		}

	}
}