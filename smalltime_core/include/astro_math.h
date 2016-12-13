#pragma once
#ifndef _ASTRO_MATH_
#define _ASTRO_MATH_

#include "core_decls.h"
#include <array>

namespace smalltime
{
	namespace math
	{

		// Complete location description used for some calendar calculations
		struct Locale
		{
			RD lattitude;
			RD longitude;
			RD elevation;
			RD zone;
		};

		//=======================================
		// Calculate the local mean time zone
		//========================================
		inline RD ZoneFromLongitude(RD longitude)
		{
			return longitude / 360.0;
		}

		//====================================================
		// Calculate universal time from local mean time
		//====================================================
		inline RD UniversalFromLocal(RD local_time, Locale locale)
		{
			return local_time - ZoneFromLongitude(locale.longitude);
		}

		//====================================================
		// Calculate local mean time from universal time
		//====================================================
		inline RD LocalFromUniversal(RD unv_time, Locale locale)
		{
			return unv_time + ZoneFromLongitude(locale.longitude);
		}

		//====================================================
		// Calculate standard time from universal
		//====================================================
		inline RD StandardFromUniversal(RD unv_time, Locale locale)
		{
			return unv_time + locale.zone;
		}

		//====================================================
		// Calculate universal from standard
		//====================================================
		inline RD UniversalFromStandard(RD std_time, Locale locale)
		{
			return std_time - locale.zone;
		}


	}
}

#endif