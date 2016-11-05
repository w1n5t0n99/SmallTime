#pragma once
#ifndef _INVALIDFIELDEXCEPTION_
#define _INVALIDFIELDEXCEPTION_

#include <stdexcept>
#include <string>

namespace smalltime
{
	class InvalidFieldException : public std::runtime_error
	{
	public:
		//============================================
		// Ctor
		//============================================
		InvalidFieldException(const std::string& message) : std::runtime_error(message), m_msg(message)
		{

		}

		//======================================
		// Dtor
		//======================================
		virtual ~InvalidFieldException() {};

		//=========================================
		// return error message
		//==========================================
		virtual const char* what() const
		{
			return m_msg.c_str();
		}

	private:
		std::string m_msg;
	};
}

#endif