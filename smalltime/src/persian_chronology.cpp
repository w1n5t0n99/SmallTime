#include "../include/persian_chronology.h"
#include "../include/chrono_decls.h"
#include "../include/cal_math.h"
#include "../include/time_math.h"

#include <float_util.h>

namespace smalltime
{
	namespace chrono
	{
		// Epoch = FixedFromJulian(622, 3, 19)
		RD KPERSIAN_EPOCH = 226896.0;
		// Older value used for calculation estimations
		RD KMEAN_TROPICAL_YEAR = 365.242189;


	}
}