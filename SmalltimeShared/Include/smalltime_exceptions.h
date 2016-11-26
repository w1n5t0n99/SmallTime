#pragma once
#ifndef _SMALLTIME_EXCEPTIONS_
#define _SMALLTIME_EXCEPTIONS_

#include <stdexcept>
#include <string>
#include <sstream>
#include <BasicDateTime.h>

namespace smalltime
{
	class InvalidFieldException : public std::runtime_error
	{
	public:
		//============================================
		// Ctor
		//============================================
		InvalidFieldException(const std::string& message) : std::runtime_error(message), except_msg_(message)
		{

		}

		//=========================================
		// return error message
		//==========================================
		virtual const char* what() const
		{
			return except_msg_.c_str();
		}

	private:
		std::string except_msg_;
	};

	class TimeZoneAmbigMultiException : public std::runtime_error
	{
	public:
		//===========================================
		// Ctor
		//============================================
		TimeZoneAmbigMultiException(BasicDateTime<> earliest_rule_dt, BasicDateTime<> latest_rule_dt) : std::runtime_error("Ambigous Multiple TimeZones"), except_msg_("Ambigous multiple timezones"),
			earliest_rule_dt_(earliest_rule_dt), latest_rule_dt_(latest_rule_dt)
		{
			std::stringstream sstr;

			sstr << except_msg_ << " between " << earliest_rule_dt_.getYear() << "/" << earliest_rule_dt_.getMonth() << "/" << earliest_rule_dt_.getDay() << "T" << earliest_rule_dt_.getHour() <<
				":" << earliest_rule_dt_.getMinute() << ":" << earliest_rule_dt_.getSecond() << ":" << earliest_rule_dt_.getMillisecond() << " and " << latest_rule_dt_.getYear() << "/" << latest_rule_dt_.getMonth() << "/" << latest_rule_dt_.getDay() << "T" << latest_rule_dt_.getHour()
				<< ":" << latest_rule_dt_.getMinute() << ":" << latest_rule_dt_.getSecond() << ":" << latest_rule_dt_.getMillisecond();

			if (earliest_rule_dt_.getType() == tz::TimeType_Wall)
				sstr << " local";
			else if (earliest_rule_dt_.getType() == tz::TimeType_Utc)
				sstr << " utc";
			else
				sstr << " some how its in STD that shouldn't happen!";


			except_msg_ = sstr.str();
		}

		//=========================================
		// return error message
		//==========================================
		virtual const char* what() const
		{
			return except_msg_.c_str();	
		}

	private:
		std::string except_msg_;
		BasicDateTime<> earliest_rule_dt_, latest_rule_dt_;

	};

	class TimeZoneAmbigNoneException : public std::runtime_error
	{
	public:
		//===========================================
		// Ctor
		//============================================
		TimeZoneAmbigNoneException(BasicDateTime<> earliest_rule_dt, BasicDateTime<> latest_rule_dt) : std::runtime_error("Ambigous Gap In TimeZones"), except_msg_("Ambigous gap in timezones"),
			earliest_rule_dt_(earliest_rule_dt), latest_rule_dt_(latest_rule_dt)
		{
			std::stringstream sstr;
			sstr << except_msg_ << " between " << earliest_rule_dt_.getYear() << "/" << earliest_rule_dt_.getMonth() << "/" << earliest_rule_dt_.getDay() << "T" << earliest_rule_dt_.getHour() <<
				":" << earliest_rule_dt_.getMinute() << ":" << earliest_rule_dt_.getSecond() << ":" << earliest_rule_dt_.getMillisecond() << " and " << latest_rule_dt_.getYear() << "/" << latest_rule_dt_.getMonth() << "/" << latest_rule_dt_.getDay() << "T" << latest_rule_dt_.getHour()
				<< ":" << latest_rule_dt_.getMinute() << ":" << latest_rule_dt_.getSecond() << ":" << latest_rule_dt_.getMillisecond();

			if (earliest_rule_dt_.getType() == tz::TimeType_Wall)
				sstr << " local";
			else if (earliest_rule_dt_.getType() == tz::TimeType_Utc)
				sstr << " some how its in UTC that shouldn't happen!";
			else
				sstr << " some how its in STD that shouldn't happen!";

			except_msg_ = sstr.str();
		}

		//=========================================
		// return error message
		//==========================================
		virtual const char* what() const
		{
			return except_msg_.c_str();
		}

	private:
		std::string except_msg_;
		BasicDateTime<> earliest_rule_dt_, latest_rule_dt_;
	};

}

#endif