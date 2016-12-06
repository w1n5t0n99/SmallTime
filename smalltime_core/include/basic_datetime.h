#pragma once
#ifndef _BASICDATETIME_
#define _BASICDATETIME_

#include <ostream>

#include "core_decls.h"
#include "tz_decls.h"
#include "iso_chronology.h"

namespace smalltime
{

	template <typename T = chrono::IsoChronology>
	class BasicDateTime
	{
		public:
		BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, tz::TimeType tmType);
		BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, tz::TimeType tmType);
		BasicDateTime(RD rd, tz::TimeType tmType);

		int GetYear() const { return ymd_[0]; }
		int GetMonth() const { return ymd_[1]; }
		int GetDay() const { return ymd_[2]; }

		int GetHour() const { return hms_[0]; }
		int GetMinute() const { return hms_[1]; }
		int GetSecond() const { return hms_[2]; }
		int GetMillisecond() const { return hms_[3]; }

		RD GetFixed() const { return fixed_; }
		tz::TimeType GetType() const { return time_type_; }

	private:
		YMD ymd_;
		HMS hms_;
		RD fixed_;
		tz::TimeType time_type_;

		static T KCHRONOLOGY;
			
	};

	//=============================================
	// Stream operator overload
	//==============================================
	template <typename T>
	std::ostream& operator<< (std::ostream& stream, const BasicDateTime<T> rhs)
	{
		return stream << rhs.GetYear() << '/' << rhs.GetMonth() << '/' << rhs.GetDay() << 'T' << rhs.GetHour() << ':' << rhs.GetMinute()
			<< ':' << rhs.GetSecond() << ':' << rhs.GetMillisecond();
	}

	//==================================
	// Init static member
	//==================================
	template <typename T = chrono::IsoChronology>
	T BasicDateTime<T>::KCHRONOLOGY;

	//================================================
	// Ctor - create date from fields
	//================================================
	template <typename T = chrono::IsoChronology>
	BasicDateTime<T>::BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, tz::TimeType tmType)
	{
		fixed_ = KCHRONOLOGY.FixedFromYmd(year, month, day);
		fixed_ += KCHRONOLOGY.FixedFromTime(hour, minute, second, millisecond);

		ymd_ = { year, month, day };
		hms_ = { hour, minute, second, millisecond };
		time_type_ = tmType;
	}

	//====================================================
	// Ctor - create date from fields relative to
	//====================================================
	template <typename T = chrono::IsoChronology>
	BasicDateTime<T>::BasicDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, RelSpec rel, tz::TimeType tmType)
	{
		fixed_ = KCHRONOLOGY.FixedFromYmd(year, month, day);
		fixed_ = KCHRONOLOGY.FixedRelativeTo(fixed_, rel);
		fixed_ += KCHRONOLOGY.FixedFromTime(hour, minute, second, millisecond);

		// check if fields have been altered
		ymd_ = KCHRONOLOGY.YmdFromFixed(fixed_);
		hms_ = KCHRONOLOGY.TimeFromFixed(fixed_);
		time_type_ = tmType;
	}

	//====================================================
	// Ctor - create date from fixed date
	//====================================================
	template <typename T = chrono::IsoChronology>
	BasicDateTime<T>::BasicDateTime(RD rd, tz::TimeType tmType)
	{
		fixed_ = rd;
		ymd_ = KCHRONOLOGY.YmdFromFixed(rd);
		hms_ = KCHRONOLOGY.TimeFromFixed(rd);

		time_type_ = tmType;
	}

}

#endif

