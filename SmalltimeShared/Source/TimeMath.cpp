#include "../Include/TimeMath.h"
#include "../Include/MathUtils.h"

namespace smalltime
{
	namespace math
	{
		//=====================================================
		// convert RD time format to time fields
		//======================================================
		HMS hmsFromRd(RD rd)
		{

			RD tmOnly = extractTime(rd);
			//Make sure value is rounded to closest whole second
			RD timeInMillis = std::round(tmOnly * MILLISECONDS_IN_DAY);

			RD hours = timeInMillis / MILLISECONDS_IN__HOUR;
			int ihours = static_cast<int>(std::floor(hours));

			RD minsInMilliseconds = quickFlMod(timeInMillis, MILLISECONDS_IN__HOUR, hours);
			RD mins = minsInMilliseconds / MILLISECONDS_IN_MINUTE;
			int imins = static_cast<int>(std::floor(mins));

			RD secsInMilliseconds = quickFlMod(minsInMilliseconds, MILLISECONDS_IN_MINUTE, mins);
			RD secs = secsInMilliseconds / MILLISECONDS_IN_SECOND;
			int isecs = static_cast<int>(std::floor(secs));

			RD millis = quickFlMod(secsInMilliseconds, MILLISECONDS_IN_SECOND, secs);
			int imillis = static_cast<int>(std::floor(millis));

			return{ ihours, imins, isecs, imillis };

		}
	}
}