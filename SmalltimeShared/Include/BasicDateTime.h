#pragma once
#ifndef _BASICDATETIME_
#define _BASICDATETIME_

#include "CoreDecls.h"
#include "TzDecls.h"
#include "IsoChronology.h"

namespace smalltime
{

	template <typename T = chrono::IsoChronology>
	class BasicDateTime
	{
		public:
		BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, tz::TimeType tmType);
		BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, tz::TimeType tmType);
		BasicDateTime(RD rd, tz::TimeType tmType);

		int getYear() const { return m_ymd[0]; }
		int getMonth() const { return m_ymd[1]; }
		int getDay() const { return m_ymd[2]; }

		int getHour() const { return m_hms[0]; }
		int getMinute() const { return m_hms[1]; }
		int getSecond() const { return m_hms[2]; }
		int getMillisecond() const { return m_hms[3]; }

		RD getRd() const { return m_rd; }
		tz::TimeType getType() const { return m_tmType; }

	private:
		YMD m_ymd;
		HMS m_hms;
		RD m_rd;
		tz::TimeType m_tmType;

		static T CHRONOLOGY;
			
	};

	//==================================
	// Init static member
	//==================================
	template <typename T = chrono::IsoChronology>
	T BasicDateTime<T>::CHRONOLOGY;

	//================================================
	// Ctor - create date from fields
	//================================================
	template <typename T = chrono::IsoChronology>
	BasicDateTime<T>::BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, tz::TimeType tmType)
	{
		m_rd = CHRONOLOGY.rdFromYmd(year, month, day);
		m_rd += CHRONOLOGY.rdFromTime(hour, minute, second, millisecond);

		m_ymd = { year, month, day };
		m_hms = { hour, minute, second, millisecond };
		m_tmType = tmType;
	}

	//====================================================
	// Ctor - create date from fields relative to
	//====================================================
	template <typename T = chrono::IsoChronology>
	BasicDateTime<T>::BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, tz::TimeType tmType)
	{
		m_rd = CHRONOLOGY.rdFromYmd(year, month, day);
		m_rd = CHRONOLOGY.rdRelativeTo(m_rd, rel);
		m_rd += CHRONOLOGY.rdFromTime(hour, minute, second, millisecond);

		// check if fields have been altered
		m_ymd = CHRONOLOGY.ymdFromRd(m_rd);
		m_hms = CHRONOLOGY.timeFromRd(m_rd);
		m_tmType = tmType;
	}

	//====================================================
	// Ctor - create date from fixed date
	//====================================================
	template <typename T = chrono::IsoChronology>
	BasicDateTime<T>::BasicDateTime(RD rd, tz::TimeType tmType)
	{
		m_rd = rd;
		m_ymd = CHRONOLOGY.ymdFromRd(rd);
		m_hms = CHRONOLOGY.timeFromRd(rd);

		m_tmType = tmType;
	}

}

#endif

