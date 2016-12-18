#include "../include/datetime_util.h"
#include <timezone_db.h>

namespace smalltime
{
	//=====================================================
	// Sets the path for the tzdb.bin to be searched for
	//======================================================
	void SetTimeZoneFilePath(std::string file_path)
	{
		tz::TimeZoneDB::SetPath(file_path);

	}

}