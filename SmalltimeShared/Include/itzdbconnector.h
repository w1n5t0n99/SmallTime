#pragma once
#ifndef _ITZDBCONNECTOR_
#define _ITZDBCONNECTOR_

#include "CoreDecls.h"
#include "TzDecls.h"

namespace smalltime
{
	namespace tz
	{
		enum class TransitionPool
		{
			KRule,
			KZone,
		};

		class ITzdbConnector
		{
		public:
			virtual const Rule* GetRuleHandle() = 0;
			virtual const Zone* GetZoneHandle() = 0;

			virtual RD* GetTransitionPool(const TransitionPool buffer_type) = 0;
			virtual void FillTransitionPool(const TransitionPool buffer_type, const RD data) = 0;
			virtual void FillTransitionPool(const TransitionPool buffer_type, int size, const RD data) = 0;

			virtual void ClearTransitionPool(const TransitionPool buffer_type) = 0;
			virtual void ClearTransitionPool(const TransitionPool buffer_type, int size) = 0;

			virtual Rules FindRules(const std::string& ruleName) = 0;
			virtual Rules FindRules(uint32_t ruleId) = 0;

			virtual Zones FindZones(const std::string& zoneName) = 0;
			virtual Zones FindZones(uint32_t zoneId) = 0;

		};
	}
}

#endif
