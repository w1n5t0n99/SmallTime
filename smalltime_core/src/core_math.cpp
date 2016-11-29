#include "../include/core_math.h"

namespace smalltime
{
	namespace math
	{
		
		/*
		//==============================================================================
		// Return the complement of the fractional part of floating point
		//==============================================================================
		RD getComplementOfFractional(RD num)
		{
			auto frac = extractTime(num);
			return 1.0 - frac;
		}
		
		*/

		//======================================================================
		// hash string to create unique ID
		//========================================================================
		uint32_t GetUniqueID(const std::string& str)
		{
			//uint32_t retID = 0;
			//MurmurHash3_x86_32(str.c_str(), str.size(), 0, &retID);

			//return retID;
			return static_cast<uint32_t>(std::hash<std::string>{}(str));

		}

		//=====================================================================
		// Store first 4 chars of string into uint32_t else fill with 0
		//=====================================================================
		uint32_t Pack4Chars(std::string str)
		{
			size_t strSize = str.size();
			strSize = ((strSize > 4) ? 4 : strSize);
			size_t rem = 4 - strSize;
			// Pad string so it has atleast 4 chars
			for (unsigned int i = 0; i < rem; ++i)
				str.push_back('0');

			return (str[0] << 24) + (str[1] << 16) + (str[2] << 8) + str[3];

		}

		//=====================================================================
		// Store first 8 chars of string into uint32_t else fill with 0
		//=====================================================================
		uint64_t Pack8Chars(std::string str)
		{
			size_t strSize = str.size();
			strSize = ((strSize > 8) ? 8 : strSize);
			size_t rem = 8 - strSize;
			// Pad string so it has atleast 8 chars
			for (unsigned int i = 0; i < rem; ++i)
				str.push_back('0');

			return  (static_cast<uint64_t>(str[0]) << 56) + (static_cast<uint64_t>(str[1]) << 48) + (static_cast<uint64_t>(str[2]) << 40) + (static_cast<uint64_t>(str[3]) << 32)
				+ (static_cast<uint64_t>(str[4]) << 24) + (static_cast<uint64_t>(str[5]) << 16) + (static_cast<uint64_t>(str[6]) << 8) + static_cast<uint64_t>(str[7]);
		}

		//============================================================
		// Convert packed chars back to string
		//=============================================================
		std::string Unpack4Chars(uint32_t num)
		{
			std::string str;
			for (unsigned int i = 4; i > 0; --i)
				str.push_back(static_cast<char>((num >> (8 * (i - 1))) & 0xff));

			return str;
		}

		//============================================================
		// Convert packed chars back to string
		//=============================================================
		std::string Unpack8Chars(uint64_t num)
		{
			std::string str;
			for (unsigned int i = 8; i > 0; --i)
				str.push_back(static_cast<char>((num >> (8 * (i - 1))) & 0xffll));

			return str;
		}

	}
}