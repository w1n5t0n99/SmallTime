#pragma once
#ifndef _INVALIDFIELDEXCEPTION_
#define _INVALIDFIELDEXCEPTION_

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
		InvalidFieldException(const std::string& message) : std::runtime_error(message), msg_(message)
		{

		}

		//=========================================
		// return error message
		//==========================================
		virtual const char* what() const
		{
			return msg_.c_str();
		}

	private:
		std::string msg_;
	};

	class TimeZoneAmbigMultiException : public std::runtime_error
	{
	public:
		//===========================================
		// Ctor
		//============================================
		TimeZoneAmbigMultiException(BasicDateTime<> earliest_rule_dt, BasicDateTime<> latest_rule_dt) : std::runtime_error("Ambigous Multiple TimeZones"), msg_("Ambigous Multiple TimeZones")
		{

		}

		//=========================================
		// return error message
		//==========================================
		virtual const char* what() const
		{
			return msg_.c_str();
		}

	private:
		std::string msg_;

	};

	class TimeZoneAmbigNoneException : public std::runtime_error
	{
	public:
		//===========================================
		// Ctor
		//============================================
		TimeZoneAmbigNoneException(BasicDateTime<> earliest_rule_dt, BasicDateTime<> latest_rule_dt) : std::runtime_error("Ambigous Gap In TimeZones"), msg_("Ambigous Gap In TimeZones")
		{

		}

		//=========================================
		// return error message
		//==========================================
		virtual const char* what() const
		{
			return msg_.c_str();
		}

	private:
		std::string msg_;

	};

}

#endif