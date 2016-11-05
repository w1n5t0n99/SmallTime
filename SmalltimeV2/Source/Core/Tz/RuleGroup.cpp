#include "RuleGroup.h"

namespace smalltime
{
	namespace tz
	{

		//=======================================================
		// Calculate rule transition to the day only
		//=======================================================
		RD RuleGroup::calculateFastTransition(const Rule* rule, int transitionYear)
		{
			if (rule->dayType == DayType_Dom)
			{
				return BasicDateTime<>(transitionYear, rule->month, rule->day, 0, 0, 0, 0, TimeType_Wall).getRd();
			}
			else if (rule->dayType == DayType_SunGE)
			{
				return BasicDateTime<>(transitionYear, rule->month, rule->day, 0, 0, 0, 0, RelSpec::SunOnOrAfter, TimeType_Wall).getRd();
			}
			else
			{

			}
		}

	}
}