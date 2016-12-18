#pragma once
#ifndef _TIMEZONE_DB_
#define _TIMEZONE_DB_

#include "core_decls.h"
#include "tz_decls.h"

#include <memory>
#include <string>

namespace smalltime
{
	namespace tz
	{
		class TimeZoneDB
		{
		public:
			
			static void SetPath(std::string path);
			static void Init();
			
			const Rule* const GetRuleHandle();
			const Zone* const GetZoneHandle();

			Rules FindRules(const std::string& ruleName);
			Rules FindRules(uint32_t rule_id);

			Zones FindZones(const std::string& zoneName);
			Zones FindZones(uint32_t zone_id);
			
		private:
			std::unique_ptr<RD[]> zone_buffer_;
			std::unique_ptr<RD[]> rule_buffer_;

			Zones BinarySearchZones(uint32_t zone_id, int size);
			Rules BinarySearchRules(uint32_t rule_id, int size);

			static std::unique_ptr<Zone[]> zone_arr_;
			static std::unique_ptr<Rule[]> rule_arr_;
			static std::unique_ptr<Zones[]> zone_lookup_arr_;
			static std::unique_ptr<Rules[]> rule_lookup_arr_;

			static int zone_size_, rule_size_, zone_lookup_size_, rule_lookup_size_;
			static bool initialized_;

			static std::string path_;
		};
	}
}

#endif



