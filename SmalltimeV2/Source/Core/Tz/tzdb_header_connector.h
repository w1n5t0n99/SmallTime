#pragma once
#ifndef _TZDBHEADERCONNECTOR_
#define _TZDBHEADERCONNECTOR_

#include <CoreDecls.h>
#include <TzDecls.h>
#include <itzdbconnector.h>

namespace smalltime
{
	namespace tz
	{
		class TzdbHeaderConnector : public ITzdbConnector
		{
		public:
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
		};
	}
}

#endif
