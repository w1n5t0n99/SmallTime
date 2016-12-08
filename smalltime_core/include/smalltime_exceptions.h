#pragma once
#ifndef _SMALLTIME_EXCEPTIONS_
#define _SMALLTIME_EXCEPTIONS_

#include <stdexcept>
#include <string>
#include <sstream>
#include "basic_datetime.h"

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

	class InvalidTimeZoneException : public std::runtime_error
	{
	public:
		//============================================
		// Ctor
		//============================================
		InvalidTimeZoneException(const std::string& name) : std::runtime_error("Timezone name not found: "), except_msg_("Timezone name not found: "), name_(name)
		{
			std::stringstream sstr;

			sstr << except_msg_ << name;
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
		std::string name_;
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

			sstr << except_msg_ << " between " << earliest_rule_dt << " and " <<latest_rule_dt;

			if (earliest_rule_dt_.GetType() == tz::KTimeType_Wall)
				sstr << " local";
			else if (earliest_rule_dt_.GetType() == tz::KTimeType_Std)
				sstr << " std";
			else
				sstr << " some how its in utc that shouldn't happen!";


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
			sstr << except_msg_ << " between " << earliest_rule_dt << " and " << latest_rule_dt;

			if (earliest_rule_dt_.GetType() == tz::KTimeType_Wall)
				sstr << " local";
			else if (earliest_rule_dt_.GetType() == tz::KTimeType_Std)
				sstr << " std";
			else
				sstr << " some how its in utc that shouldn't happen!";

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