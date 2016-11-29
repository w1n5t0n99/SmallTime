#include "../Include/TimeMath.h"
#include "../Include/MathUtils.h"

namespace smalltime
{
	namespace math
	{
		//=====================================================
		// convert RD time format to time fields
		//======================================================
		HMS HmsFromFixed(RD rd)
		{

			RD tmOnly = ExtractTime(rd);
			//Make sure value is rounded to closest whole second
			RD timeInMillis = std::round(tmOnly * KMILLISECONDS_IN_DAY);

			RD hours = timeInMillis / KMILLISECONDS_IN__HOUR;
			int ihours = static_cast<int>(std::floor(hours));

			RD minsInMilliseconds = QuickFlMod(timeInMillis, KMILLISECONDS_IN__HOUR, hours);
			RD mins = minsInMilliseconds / KMILLISECONDS_IN_MINUTE;
			int imins = static_cast<int>(std::floor(mins));

			RD secsInMilliseconds = QuickFlMod(minsInMilliseconds, KMILLISECONDS_IN_MINUTE, mins);
			RD secs = secsInMilliseconds / KMILLISECONDS_IN_SECOND;
			int isecs = static_cast<int>(std::floor(secs));

			RD millis = QuickFlMod(secsInMilliseconds, KMILLISECONDS_IN_SECOND, secs);
			int imillis = static_cast<int>(std::floor(millis));

			return{ ihours, imins, isecs, imillis };

		}
	}
}