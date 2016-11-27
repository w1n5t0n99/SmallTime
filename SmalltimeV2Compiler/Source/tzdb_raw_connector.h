#pragma once
#ifndef _TZDB_RAW_CONNECTOR_
#define _TZDB_RAW_CONNECTOR_

#include <CoreDecls.h>
#include <TzDecls.h>
#include "CompDecls.h"
#include <itzdbconnector.h>
#include <vector>
#include <array>

namespace smalltime
{
	namespace comp
	{
		class TzdbRawConnector : public tz::ITzdbConnector
		{
		public:
			TzdbRawConnector(const MetaData& tzdb_meta, const std::vector<tz::Zone>& vec_zone, const std::vector<tz::Rule>& vec_rule,
				const std::vector<tz::Zones>& vec_zone_lookup, const std::vector<tz::Rules>& vec_rule_lookup);

			virtual const tz::Rule* GetRuleHandle() override;
			virtual const  tz::Zone* GetZoneHandle() override;

			virtual RD* GetTransitionPool(const  tz::TransitionPool buffer_type) override;
			virtual void FillTransitionPool(const  tz::TransitionPool buffer_type, const RD data) override;
			virtual void FillTransitionPool(const  tz::TransitionPool buffer_type, int size, const RD data) override;

			virtual void ClearTransitionPool(const  tz::TransitionPool buffer_type) override;
			virtual void ClearTransitionPool(const  tz::TransitionPool buffer_type, int size) override;

			virtual  tz::Rules FindRules(const std::string& rule_name) override;
			virtual  tz::Rules FindRules(uint32_t rule_id) override;

			virtual  tz::Zones FindZones(const std::string& zone_name) override;
			virtual  tz::Zones FindZones(uint32_t zone_id) override;

		private:
			const std::vector< tz::Zone>& vec_zone_;
			const std::vector< tz::Rule>& vec_rule_;
			const std::vector<tz::Zones>& vec_zone_lookup_;
			const std::vector<tz::Rules>& vec_rule_lookup_;
			const MetaData& meta_data_;

			std::vector<RD> zone_pool_;
			std::vector<RD> rule_pool_;

		};
	}
}

#endif