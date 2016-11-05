#pragma once
#ifndef _LOCALDATETIME_
#define _LOCALDATETIME_

#include <CoreDecls.h>
#include <IsoChronology.h>
#include "../Exceptions.h"

#include <array>

namespace smalltime
{
	template <typename T = chrono::IsoChronology>
	class LocalDateTime
	{
	public:
		LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);
		LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel);
		LocalDateTime(RD rd);

		int getYear() const { return m_ymd[0]; }
		int getMonth() const { return m_ymd[1]; }
		int getDay() const { return m_ymd[2]; }

		int getHour() const { return m_hms[0]; }
		int getMinute() const { return m_hms[1]; }
		int getSecond() const { return m_hms[2]; }
		int getMillisecond() const { return m_hms[3]; }

		RD getRd() const { return m_rd; }

	private:
		YMD m_ymd;
		HMS m_hms;
		RD m_rd;

		static T CHRONOLOGY;
	};

	//==================================
	// Init static member
	//==================================
	template <typename T = chrono::IsoChronology>
	T LocalDateTime<T>::CHRONOLOGY;

	//================================================
	// Ctor - create date from fields
	//================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		m_rd = CHRONOLOGY.rdFromYmd(year, month, day);
		m_rd += CHRONOLOGY.rdFromTime(hour, minute, second, millisecond);

		m_ymd = CHRONOLOGY.ymdFromRd(m_rd);
		// check if valid date
		if (year != m_ymd[0] || month != m_ymd[1] || day != m_ymd[2])
			throw InvalidFieldException("Invalid field or fields");
		// check if valid time
		m_hms = CHRONOLOGY.timeFromRd(m_rd);
		if(hour != m_hms[0] || minute != m_hms[1] || second != m_hms[2] || millisecond != m_hms[3])
			throw InvalidFieldException("Invalid field or fields");

	}

	//====================================================
	// Ctor - create date from fields relative to
	//====================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel)
	{
		m_rd = CHRONOLOGY.rdFromYmd(year, month, day);
		m_ymd = CHRONOLOGY.ymdFromRd(m_rd);
		// check if valid date
		if (year != m_ymd[0] || month != m_ymd[1] || day != m_ymd[2])
			throw InvalidFieldException("Invalid field or fields");

		m_rd = CHRONOLOGY.rdRelativeTo(m_rd, rel);
		m_ymd = CHRONOLOGY.ymdFromRd(m_rd);

		m_rd += CHRONOLOGY.rdFromTime(hour, minute, second, millisecond);
		// check if valid time
		m_hms = CHRONOLOGY.timeFromRd(m_rd);
		if (hour != m_hms[0] || minute != m_hms[1] || second != m_hms[2] || millisecond != m_hms[3])
			throw InvalidFieldException("Invalid field or fields");
	}

	//====================================================
	// Ctor - create date from fixed date
	//====================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(RD rd)
	{
		m_ymd = CHRONOLOGY.ymdFromRd(rd);
		m_rd = CHRONOLOGY.rdFromYmd(m_ymd[0], m_ymd[1], m_ymd[2]);

		m_hms = CHRONOLOGY.timeFromRd(rd);
		m_rd += CHRONOLOGY.rdFromTime(m_hms[0], m_hms[1], m_hms[2], m_hms[3]);

		// check if valid date
		if (m_rd != rd)
			throw InvalidFieldException("Invalid field or fields");
	}

	//==================================================================
	// Explicit specialization for Iso calendar
	// allows initialization by year/week/day and year/day format
	//===================================================================
	template <>
	class LocalDateTime<chrono::IsoChronology>
	{
	public:
		LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);
		LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel);
		LocalDateTime(RD rd);

		//LocalDateTime(int year, int week, int day, int hour, int minute, int second, int millisecond);
		//LocalDateTime(int year, int day, int hour, int minute, int second, int millisecond);


		int getYear() const { return m_ymd[0]; }
		int getMonth() const { return m_ymd[1]; }
		int getDay() const { return m_ymd[2]; }

		int getWeekYear() const { return m_ywd[0]; }
		int getWeekOfWeekYear() const { return m_ywd[1]; }
		int getDayOfWeekYear() const { return m_ywd[2]; }

		int getDayOfYear() const { return m_dayOfYear; }

		int getHour() const { return m_hms[0]; }
		int getMinute() const { return m_hms[1]; }
		int getSecond() const { return m_hms[2]; }
		int getMillisecond() const { return m_hms[3]; }

		int getWeekOfMonth() const { return m_weekOfMonth; }
		bool isLeapYear() const { return m_leapYear; }

		RD getRd() const { return m_rd; }

	private:
		YMD m_ymd;
		YWD m_ywd;
		HMS m_hms;
		int m_dayOfYear, m_weekOfMonth;
		bool m_leapYear;
		RD m_rd;

		static chrono::IsoChronology CHRONOLOGY;
	};

	//==================================
	// Init static member
	//==================================
	chrono::IsoChronology LocalDateTime<chrono::IsoChronology>::CHRONOLOGY;

	//================================================
	// Ctor - create date from fields
	//================================================
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		m_rd = CHRONOLOGY.rdFromYmd(year, month, day);
		m_rd += CHRONOLOGY.rdFromTime(hour, minute, second, millisecond);

		m_ymd = CHRONOLOGY.ymdFromRd(m_rd);
		// check if valid date
		if (year != m_ymd[0] || month != m_ymd[1] || day != m_ymd[2])
			throw InvalidFieldException("Invalid field or fields");
		// check if valid time
		m_hms = CHRONOLOGY.timeFromRd(m_rd);
		if (hour != m_hms[0] || minute != m_hms[1] || second != m_hms[2] || millisecond != m_hms[3])
			throw InvalidFieldException("Invalid field or fields");

		// since this should be a valid date we can obtain the other data
		m_ywd = CHRONOLOGY.ywdFromRd(m_rd);
		m_leapYear = CHRONOLOGY.isLeapYear(m_ymd[0]);
		m_weekOfMonth = CHRONOLOGY.weekOfMonth(m_ymd, m_rd);
		auto yd = CHRONOLOGY.ydFromRd(m_rd);
		m_dayOfYear = yd[1];

	}

	//====================================================
	// Ctor - create date from fields relative to
	//====================================================
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel)
	{
		m_rd = CHRONOLOGY.rdFromYmd(year, month, day);
		m_ymd = CHRONOLOGY.ymdFromRd(m_rd);
		// check if valid date
		if (year != m_ymd[0] || month != m_ymd[1] || day != m_ymd[2])
			throw InvalidFieldException("Invalid field or fields");

		m_rd = CHRONOLOGY.rdRelativeTo(m_rd, rel);
		m_ymd = CHRONOLOGY.ymdFromRd(m_rd);

		m_rd += CHRONOLOGY.rdFromTime(hour, minute, second, millisecond);
		// check if valid time
		m_hms = CHRONOLOGY.timeFromRd(m_rd);
		if (hour != m_hms[0] || minute != m_hms[1] || second != m_hms[2] || millisecond != m_hms[3])
			throw InvalidFieldException("Invalid field or fields");

		// since this should be a valid date we can obtain the other data
		m_ywd = CHRONOLOGY.ywdFromRd(m_rd);
		m_leapYear = CHRONOLOGY.isLeapYear(m_ymd[0]);
		m_weekOfMonth = CHRONOLOGY.weekOfMonth(m_ymd, m_rd);
		auto yd = CHRONOLOGY.ydFromRd(m_rd);
		m_dayOfYear = yd[1];
	}

	//====================================================
	// Ctor - create date from fixed date
	//====================================================
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(RD rd)
	{
		m_ymd = CHRONOLOGY.ymdFromRd(rd);
		m_rd = CHRONOLOGY.rdFromYmd(m_ymd[0], m_ymd[1], m_ymd[2]);

		m_hms = CHRONOLOGY.timeFromRd(rd);
		m_rd += CHRONOLOGY.rdFromTime(m_hms[0], m_hms[1], m_hms[2], m_hms[3]);

		// check if valid date
		if (m_rd != rd)
			throw InvalidFieldException("Invalid field or fields");

		// since this should be a valid date we can obtain the other data
		m_ywd = CHRONOLOGY.ywdFromRd(m_rd);
		m_leapYear = CHRONOLOGY.isLeapYear(m_ymd[0]);
		m_weekOfMonth = CHRONOLOGY.weekOfMonth(m_ymd, m_rd);
		auto yd = CHRONOLOGY.ydFromRd(m_rd);
		m_dayOfYear = yd[1];
	}

}

#endif
