#pragma once
#ifndef _LOCALDATETIME_
#define _LOCALDATETIME_

#include <core_decls.h>
#include <iso_chronology.h>
#include <smalltime_exceptions.h>

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

		int GetYear() const { return ymd_[0]; }
		int GetMonth() const { return ymd_[1]; }
		int GetDay() const { return ymd_[2]; }

		int GetHour() const { return hms_[0]; }
		int GetMinute() const { return hms_[1]; }
		int GetSecond() const { return hms_[2]; }
		int GetMillisecond() const { return hms_[3]; }

		RD GetFixed() const { return fixed_; }

	private:
		YMD ymd_;
		HMS hms_;
		RD fixed_;

		static T KCHRONOLOGY;
	};

	//==================================
	// Init static member
	//==================================
	template <typename T = chrono::IsoChronology>
	T LocalDateTime<T>::KCHRONOLOGY;

	//================================================
	// Ctor - create date from fields
	//================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		fixed_ = KCHRONOLOGY.FixedFromYmd(year, month, day);
		fixed_ += KCHRONOLOGY.FixedFromTime(hour, minute, second, millisecond);

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		// check if valid date
		if (year != ymd_[0] || month != ymd_[1] || day != ymd_[2])
			throw InvalidFieldException("Invalid field or fields");
		// check if valid time
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		if(hour != hms_[0] || minute != hms_[1] || second != hms_[2] || millisecond != hms_[3])
			throw InvalidFieldException("Invalid field or fields");

	}

	//====================================================
	// Ctor - create date from fields relative to
	//====================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel)
	{
		fixed_ = KCHRONOLOGY.FixedFromYmd(year, month, day);
		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		// check if valid date
		if (year != ymd_[0] || month != ymd_[1] || day != ymd_[2])
			throw InvalidFieldException("Invalid field or fields");

		fixed_ = KCHRONOLOGY.FixedRelativeTo(fixed_, rel);
		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);

		fixed_ += KCHRONOLOGY.FixedFromTime(hour, minute, second, millisecond);
		// check if valid time
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		if (hour != hms_[0] || minute != hms_[1] || second != hms_[2] || millisecond != hms_[3])
			throw InvalidFieldException("Invalid field or fields");
	}

	//====================================================
	// Ctor - create date from fixed date
	//====================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(RD rd)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != rd)
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


		int GetYear() const { return ymd_[0]; }
		int GetMonth() const { return ymd_[1]; }
		int GetDay() const { return ymd_[2]; }

		int GetWeekYear() const { return ywd_[0]; }
		int GetWeekOfWeekYear() const { return ywd_[1]; }
		int GetDayOfWeekYear() const { return ywd_[2]; }

		int GetDayOfYear() const { return day_of_year_; }

		int GetHour() const { return hms_[0]; }
		int GetMinute() const { return hms_[1]; }
		int GetSecond() const { return hms_[2]; }
		int GetMillisecond() const { return hms_[3]; }

		int GetWeekOfMonth() const { return week_of_month_; }
		bool IsLeapYear() const { return leap_year_; }

		RD GetFixed() const { return fixed_; }

	private:
		YMD ymd_;
		YWD ywd_;
		HMS hms_;
		int day_of_year_, week_of_month_;
		bool leap_year_;
		RD fixed_;

		static chrono::IsoChronology KCHRONOLOGY;
	};

	//==================================
	// Init static member
	//==================================
	chrono::IsoChronology LocalDateTime<chrono::IsoChronology>::KCHRONOLOGY;

	//================================================
	// Ctor - create date from fields
	//================================================
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		fixed_ = KCHRONOLOGY.FixedFromYmd(year, month, day);
		fixed_ += KCHRONOLOGY.FixedFromTime(hour, minute, second, millisecond);

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		// check if valid date
		if (year != ymd_[0] || month != ymd_[1] || day != ymd_[2])
			throw InvalidFieldException("Invalid field or fields");
		// check if valid time
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		if (hour != hms_[0] || minute != hms_[1] || second != hms_[2] || millisecond != hms_[3])
			throw InvalidFieldException("Invalid field or fields");

		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];

	}

	//====================================================
	// Ctor - create date from fields relative to
	//====================================================
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel)
	{
		fixed_ = KCHRONOLOGY.FixedFromYmd(year, month, day);
		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		// check if valid date
		if (year != ymd_[0] || month != ymd_[1] || day != ymd_[2])
			throw InvalidFieldException("Invalid field or fields");

		fixed_ = KCHRONOLOGY.FixedRelativeTo(fixed_, rel);
		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);

		fixed_ += KCHRONOLOGY.FixedFromTime(hour, minute, second, millisecond);
		// check if valid time
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		if (hour != hms_[0] || minute != hms_[1] || second != hms_[2] || millisecond != hms_[3])
			throw InvalidFieldException("Invalid field or fields");

		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];
	}

	//====================================================
	// Ctor - create date from fixed date
	//====================================================
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(RD rd)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != rd)
			throw InvalidFieldException("Invalid field or fields");

		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];
	}

}

#endif
