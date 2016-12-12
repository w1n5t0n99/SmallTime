#pragma once
#ifndef _DATETIME_
#define _DATETIME_

#include <array>
#include <memory>
#include <string>

#include <core_decls.h>
#include <iso_chronology.h>
#include <timezone.h>
#include <smalltime_exceptions.h>
#include "tzdb_header_connector.h"

namespace smalltime
{
	template <typename T = chrono::IsoChronology>
	class DateTime
	{
	public:
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, std::string time_zone);
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, std::string time_zone);
		DateTime(RD rd, std::string time_zone);
		DateTime(RD rd);

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
		static tz::TimeZone KTIMEZONE;
		static std::shared_ptr<tz::TzdbHeaderConnector> KTZDB_CONNECTOR;
	};

	//==================================
	// Init static member
	//==================================
	template <typename T = chrono::IsoChronology>
	T DateTime<T>::KCHRONOLOGY;

	template <typename T = chrono::IsoChronology>
	tz::TimeZone DateTime<T>::KTIMEZONE;

	template <typename T = chrono::IsoChronology>
	std::shared_ptr<tz::TzdbHeaderConnector> DateTime<T>::KTZDB_CONNECTOR = std::make_shared<tz::TzdbHeaderConnector>();

	//================================================
	// Ctor - create date from fields
	//================================================
	template <typename T = chrono::IsoChronology>
	DateTime<T>::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, std::string time_zone)
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
		
		auto offset = KTIMEZONE.FixedFromLocal(fixed_, time_zone, Choose::KError, KTZDB_CONNECTOR);
		fixed_ -= offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);

	}

	//====================================================
	// Ctor - create date from fields relative to
	//====================================================
	template <typename T = chrono::IsoChronology>
	DateTime<T>::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, std::string time_zone)
	{
		fixed_ = KCHRONOLOGY.FixedFromYmd(year, month, day);
		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		// check if valid date
		if (year != ymd_[0] || month != ymd_[1] || day != ymd_[2])
			throw InvalidFieldException("Invalid field or fields");

		fixed_ = KCHRONOLOGY.FixedRelativeTo(fixed_, rel);
		fixed_ += KCHRONOLOGY.FixedFromTime(hour, minute, second, millisecond);

		// check if valid time
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		if (hour != hms_[0] || minute != hms_[1] || second != hms_[2] || millisecond != hms_[3])
			throw InvalidFieldException("Invalid field or fields");

		auto offset = KTIMEZONE.FixedFromLocal(fixed_, time_zone, Choose::KError, KTZDB_CONNECTOR);
		fixed_ -= offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);

	}

	//====================================================
	// Ctor - create date from local fixed date
	//====================================================
	template <typename T = chrono::IsoChronology>
	DateTime<T>::DateTime(RD rd, std::string time_zone)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != rd)
			throw InvalidFieldException("Invalid field or fields");

		auto offset = KTIMEZONE.FixedFromLocal(fixed_, time_zone, Choose::KError, KTZDB_CONNECTOR);
		fixed_ -= offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		hms_ = KCHRONOLOGY.TimeFromFixed(rd);

	}

	//====================================================
	// Ctor - create date from fixed date
	//====================================================
	template <typename T = chrono::IsoChronology>
	DateTime<T>::DateTime(RD rd)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != rd)
			throw InvalidFieldException("Invalid field or fields");

		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		hms_ = KCHRONOLOGY.TimeFromFixed(rd);

	}

	//==================================================================
	// Explicit specialization for Iso calendar
	// allows initialization by year/week/day and year/day format
	//===================================================================
	template <>
	class DateTime<chrono::IsoChronology>
	{
	public:
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, std::string time_zone);
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, std::string time_zone);
		DateTime(RD rd, std::string time_zone);
		DateTime(RD rd);

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
		static tz::TimeZone KTIMEZONE;
		static std::shared_ptr<tz::TzdbHeaderConnector> KTZDB_CONNECTOR;
	};

	//==================================
	// Init static member
	//==================================
	chrono::IsoChronology DateTime<chrono::IsoChronology>::KCHRONOLOGY;
	tz::TimeZone DateTime<chrono::IsoChronology>::KTIMEZONE;
	std::shared_ptr<tz::TzdbHeaderConnector> DateTime<chrono::IsoChronology>::KTZDB_CONNECTOR = std::make_shared<tz::TzdbHeaderConnector>();

	//================================================
	// Ctor - create date from fields
	//================================================
	DateTime<chrono::IsoChronology>::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, std::string time_zone)
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

		auto offset = KTIMEZONE.FixedFromLocal(fixed_, time_zone, Choose::KError, KTZDB_CONNECTOR);
		fixed_ -= offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
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
	DateTime<chrono::IsoChronology>::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, std::string time_zone)
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

		auto offset = KTIMEZONE.FixedFromLocal(fixed_, time_zone, Choose::KError, KTZDB_CONNECTOR);
		fixed_ -= offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		// since this should be a valid date we can obtain the other data
		ywd_ = KCHRONOLOGY.YwdFromFixed(fixed_);
		leap_year_ = KCHRONOLOGY.IsLeapYear(ymd_[0]);
		week_of_month_ = KCHRONOLOGY.WeekOfMonth(ymd_, fixed_);
		auto yd = KCHRONOLOGY.YdFromFixed(fixed_);
		day_of_year_ = yd[1];
	}

	//====================================================
	// Ctor - create date from local fixed date
	//====================================================
	DateTime<chrono::IsoChronology>::DateTime(RD rd, std::string time_zone)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != rd)
			throw InvalidFieldException("Invalid field or fields");

		auto offset = KTIMEZONE.FixedFromLocal(fixed_, time_zone, Choose::KError, KTZDB_CONNECTOR);
		fixed_ -= offset;

		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		hms_ = KCHRONOLOGY.TimeFromFixed(rd);
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
	DateTime<chrono::IsoChronology>::DateTime(RD rd)
	{
		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		fixed_ = KCHRONOLOGY.FixedFromYmd(ymd_[0], ymd_[1], ymd_[2]);

		hms_ = KCHRONOLOGY.TimeFromFixed(rd);
		fixed_ += KCHRONOLOGY.FixedFromTime(hms_[0], hms_[1], hms_[2], hms_[3]);

		// check if valid date
		if (fixed_ != rd)
			throw InvalidFieldException("Invalid field or fields");

		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		hms_ = KCHRONOLOGY.TimeFromFixed(rd);

	}

	//=============================================
	// Stream operator overload
	//==============================================
	template <typename T>
	std::ostream& operator<< (std::ostream& stream, const DateTime<T> rhs)
	{
		return stream << rhs.GetYear() << '/' << rhs.GetMonth() << '/' << rhs.GetDay() << 'T' << rhs.GetHour() << ':' << rhs.GetMinute()
			<< ':' << rhs.GetSecond() << ':' << rhs.GetMillisecond();
	}

}

#endif
