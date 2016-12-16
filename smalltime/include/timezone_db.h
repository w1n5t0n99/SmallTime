#pragma once
#ifndef _TIMEZONE_DB_
#define _TIMEZONE_DB_

#include <core_decls.h>
#include <tz_decls.h>
#include <tzdb_connector_interface.h>

#include <memory>
#include <string>

namespace smalltime
{
	namespace tz
	{
		class TimezoneDB : public TzdbConnectorInterface
		{
		public:
			
			void SetPath(const std::string& path);
			void Init();

			virtual const Rule* GetRuleHandle() override;
			virtual const Zone* GetZoneHandle() override;

			virtual RD* GetTransitionPool(const TransitionPool buffer_type) override;
			virtual void FillTransitionPool(const TransitionPool buffer_type, const RD data) override;
			virtual void FillTransitionPool(const TransitionPool buffer_type, int size, const RD data) override;

			virtual void ClearTransitionPool(const TransitionPool buffer_type) override;
			virtual void ClearTransitionPool(const TransitionPool buffer_type, int size) override;

			virtual Rules FindRules(const std::string& ruleName) override;
			virtual Rules FindRules(uint32_t rule_id) override;

			virtual Zones FindZones(const std::string& zoneName) override;
			virtual Zones FindZones(uint32_t zone_id) override;

		private:
			std::unique_ptr<RD[]> zone_buffer_;
			std::unique_ptr<RD[]> rule_buffer_;

			std::unique_ptr<tz::Zone[]> zone_arr_;
			std::unique_ptr<tz::Rule[]> rule_arr_;
			std::unique_ptr<tz::Zones[]> zone_lookup_arr_;
			std::unique_ptr<tz::Rules[]> rule_lookup_arr_;

			std::string path_ = "tzdb.bin";
		};
	}
}

#endif



