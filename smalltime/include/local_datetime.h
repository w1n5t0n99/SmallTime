#pragma once
#ifndef _LOCALDATETIME_
#define _LOCALDATETIME_

#include <core_decls.h>
#include <iso_chronology.h>
#include <smalltime_exceptions.h>
#include <timezone.h>
#include <float_util.h>

#include "datetime_util.h"

#include <array>

namespace smalltime
{
	//Forward decl
	template <typename T>
	class DateTime;

	template <typename T = chrono::IsoChronology>
	class LocalDateTime
	{
	public:
		LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);
		LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RS rel);

		LocalDateTime(RD local_rd);
		LocalDateTime(RD utc_rd, const std::string& time_zone);

		template <typename U>
		LocalDateTime(const LocalDateTime<U>& other) noexcept;

		template <typename U>
		LocalDateTime(const DateTime<U>& other, const std::string& time_zone);

		template <typename U>
		LocalDateTime(const LocalDateTime<U>& other, RS rel) noexcept;

		int GetYear() const { return ymd_[0]; }
		int GetMonth() const { return ymd_[1]; }
		int GetDay() const { return ymd_[2]; }

		int GetHour() const { return hms_[0]; }
		int GetMinute() const { return hms_[1]; }
		int GetSecond() const { return hms_[2]; }
		int GetMillisecond() const { return hms_[3]; }

		RD GetFixed() const { return fixed_; }

	private:
		std::array<int, 3> ymd_;
		std::array<int, 4> hms_;
		RD fixed_;

		static T KCHRONOLOGY;
		static tz::TimeZone KTIMEZONE;
	};

	//==================================
	// Init static member
	//==================================
	template <typename T = chrono::IsoChronology>
	T LocalDateTime<T>::KCHRONOLOGY;

	template <typename T = chrono::IsoChronology>
	tz::TimeZone LocalDateTime<T>::KTIMEZONE;

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
	LocalDateTime<T>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RS rel)
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

	//===========================================================
	// Ctor - create date from fixed date interpreted as local
	//============================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(RD local_rd)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(local_rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(local_rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != local_rd)
			throw InvalidFieldException("Invalid field or fields");
	}

	//===============================================================
	// Ctor - create date from fixed date interpreted as utc
	//==============================================================
	template <typename T = chrono::IsoChronology>
	LocalDateTime<T>::LocalDateTime(RD utc_rd, const std::string& time_zone)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(utc_rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(utc_rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (!AlmostEqualRelative(fixed_, utc_rd))
			throw InvalidFieldException("Invalid field or fields");

		auto offset = KTIMEZONE.FixedOffsetFromUtc(fixed_, time_zone);
		fixed_ += offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
	}

	//=============================================================
	// create from LocalDateTime of diffrent chronology
	//============================================================
	template <typename T = chrono::IsoChronology>
	template <typename U>
	LocalDateTime<T>::LocalDateTime(const LocalDateTime<U>& other) noexcept
	{
		// We know the other DateTime must be valid if it didn't throw an exception,
		// no reason to check if fields are valid
		fixed_ = other.GetFixed();
		ymd_ = KCHRONOLOGY.YmdFromFixed(other.GetFixed());
		hms_ = KCHRONOLOGY.TimeFromFixed(other.GetFixed());
	}

	//=============================================================
	// create from a DateTime
	//============================================================
	template <typename T = chrono::IsoChronology>
	template <typename U>
	LocalDateTime<T>::LocalDateTime(const DateTime<U>& other, const std::string& time_zone)
	{
		// We know the other DateTime must be valid if it didn't throw an exception,
		// no reason to check if fields are valid
		fixed_ = other.GetFixed();
		auto offset = KTIMEZONE.FixedOffsetFromUtc(fixed_, time_zone);
		fixed_ += offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(other.GetFixed());
		hms_ = KCHRONOLOGY.TimeFromFixed(other.GetFixed());

	}

	//=================================================
	// Create relative to another LocalDateTime
	//=================================================
	template <typename T = chrono::IsoChronology>
	template <typename U>
	LocalDateTime<T>::LocalDateTime(const LocalDateTime<U>& other, RS rel) noexcept
	{
		fixed_ = other.GetFixed();
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		fixed_ = KCHRONOLOGY.FixedRelativeTo(fixed_, rel);

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);
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
		LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RS rel);

		LocalDateTime(RD local_rd);
		LocalDateTime(RD utc_rd, const std::string& time_zone);

		template <typename U>
		LocalDateTime(const LocalDateTime<U>& other) noexcept;

		template <typename U>
		LocalDateTime(const DateTime<U>& other, const std::string& time_zone);

		template <typename U>
		LocalDateTime(const LocalDateTime<U>& other, RS rel) noexcept;

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
		std::array<int, 3> ymd_;
		std::array<int, 3> ywd_;
		std::array<int, 4> hms_;
		int day_of_year_, week_of_month_, day_of_week_;
		bool leap_year_;
		RD fixed_;

		static chrono::IsoChronology KCHRONOLOGY;
		static tz::TimeZone KTIMEZONE;
	};

	//==================================
	// Init static member
	//==================================
	chrono::IsoChronology LocalDateTime<chrono::IsoChronology>::KCHRONOLOGY;
	tz::TimeZone LocalDateTime<chrono::IsoChronology>::KTIMEZONE;

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
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RS rel)
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
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(RD local_rd)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(local_rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(local_rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != local_rd)
			throw InvalidFieldException("Invalid field or fields");

		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];
	}

	//===============================================================
	// Ctor - create date from fixed date interpreted as utc
	//==============================================================
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(RD utc_rd, const std::string& time_zone)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(utc_rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(utc_rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (!AlmostEqualRelative(fixed_, utc_rd))
			throw InvalidFieldException("Invalid field or fields");

		auto offset = KTIMEZONE.FixedOffsetFromUtc(fixed_, time_zone);
		fixed_ += offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];
	}

	//==================================================================
	// Ctor - create date from LocalDateTime of diffren chronology
	//==================================================================
	template <typename U>
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(const LocalDateTime<U>& other) noexcept
	{
		// We know the other DateTime must be valid if it didn't throw an exception,
		// no reason to check if fields are valid
		fixed_ = other.GetFixed();
		ymd_ = KCHRONOLOGY.YmdFromFixed(other.GetFixed());
		hms_ = KCHRONOLOGY.TimeFromFixed(other.GetFixed());

		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];
	}

	//=============================================================
	// create from a DateTime
	//============================================================
	template <typename U>
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(const DateTime<U>& other, const std::string& time_zone)
	{
		// We know the other DateTime must be valid if it didn't throw an exception,
		// no reason to check if fields are valid
		fixed_ = other.GetFixed();
		auto offset = KTIMEZONE.FixedOffsetFromUtc(fixed_, time_zone);
		fixed_ += offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);

		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];

	}

	//=================================================
	// Create relative to another LocalDateTime
	//=================================================
	template <typename U>
	LocalDateTime<chrono::IsoChronology>::LocalDateTime(const LocalDateTime<U>& other, RS rel) noexcept
	{
		fixed_ = other.GetFixed();
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		fixed_ = KCHRONOLOGY.FixedRelativeTo(fixed_, rel);

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];
	}

	//=============================================
	// Stream operator overload
	//==============================================
	template <typename T>
	std::ostream& operator<< (std::ostream& stream, const LocalDateTime<T> rhs)
	{
		return stream << rhs.GetYear() << '/' << rhs.GetMonth() << '/' << rhs.GetDay() << 'T' << rhs.GetHour() << ':' << rhs.GetMinute()
			<< ':' << rhs.GetSecond() << ':' << rhs.GetMillisecond();
	}

}

#endif
