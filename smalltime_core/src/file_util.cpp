#include "../include/file_util.h"


namespace smalltime
{
	namespace fileutil
	{

		static const char *separators = "/\\";
		static char tmpBuffer[2048];

		//===========================================
		// check if character is a seprator
		//============================================
		bool IsSeparator(char c)
		{
			return  c == '/' || c == '\\';
		}

		//==============================================
		// strip and ending separator
		//===============================================
		std::string StripSeparator(std::string s)
		{
			while (!s.empty() && IsSeparator(s.back()))
				s.pop_back();

			return std::move(s);
		}

		//===================================================
		// append separator to end of path
		//====================================================
		std::string AddSeparator(std::string s)
		{
			if (s.empty() || IsSeparator(s.back()))
				return std::move(s);
			s += '/';
			return std::move(s);
		}

		//===============================================
		// return string without file extension
		//================================================
		std::string StripExt(std::string s)
		{

			int lastIndex = s.find_last_of(".");

			if (lastIndex == std::string::npos)
				return std::move(s);

			s.erase(lastIndex, s.size() - 1);
			return std::move(s);

		}

		//=================================================
		// return string without any path info
		//==================================================
		std::string StripParent(std::string s)
		{
			if (IsRootDirectory(s))
				return std::string();

			s = StripSeparator(s);

			std::string::size_type pos = FindFilenamePos(s);
			//if there is no filename then there's nothing to extract
			if (pos == std::string::npos)
				return std::string();

			s.erase(0, pos);
			return std::move(s);

		}


		//================================================
		// check if str is root directory
		//================================================
		bool IsRootDirectory(const std::string& s)
		{
			if (s.size() == 2 && s[1] == ':')
				return true;
			if (s.size() == 3 && s[1] == ':' && IsSeparator(s[2]))
				return true;

			// UNC style path (e.g. network drives)
			if (s.size() == 2 && IsSeparator(s[0]) && IsSeparator(s[1]))
				return true;

			return false;
		}

		//=================================================
		// find pos where the filename starts
		//====================================================
		std::string::size_type FindFilenamePos(const std::string &s)
		{
			if (s.empty() || IsSeparator(s.back()))
				return std::string::npos;

			if (s.back() == ':')
				return std::string::npos;

			std::string::size_type n = s.find_last_of(separators);
			if (n == std::string::npos)
				n = s.find_last_of(':');


			if (n == std::string::npos)
				return 0;
			else
				return n + 1;

		}

		//==================================================
		// check is relative path
		//==================================================
		bool IsRelativePath(const std::string& s)
		{
			if (s.empty() || s.size() <= 2)
				return false;

			return (!IsRootDirectory(s.substr(0, 2)));

		}

		/*
		//=====================================================
		// check if a file exists
		//=====================================================
		bool doesFileExist(const std::string& path)
		{
			return GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES;

		}
		*/

		//====================================================
		// return the file extension
		//====================================================
		std::string ExtractFileExt(std::string s)
		{

			int lastIndex = s.find_last_of(".");
			if (lastIndex == std::string::npos)
				return std::string();
			else
				return s.substr(lastIndex, s.size() - 1);

		}

		//====================================================
		// return the file parent
		//====================================================
		std::string ExtractParent(std::string s)
		{

			if (IsRootDirectory(s))
				return std::move(s);

			s = StripSeparator(s);

			std::string::size_type pos = FindFilenamePos(s);
			if (pos == std::string::npos)
				return std::move(s);
			//erase filename to end
			s.erase(pos);
			if (!IsRootDirectory(s))
				s = StripSeparator(std::move(s));

			return std::move(s);

		}

		//====================================================
		// return the base file name
		//=====================================================
		std::string  ExtractBase(std::string s)
		{
			return (StripParent(StripExt(std::move(s))));

		}

		/*
		//============================================================
		// get the absolute path from relative
		//===========================================================
		std::string toAbsolutePath(const std::string &path)
		{
			DWORD size = GetFullPathNameA(path.c_str(), sizeof(tmpBuffer), tmpBuffer, nullptr);

			if (size >= sizeof(tmpBuffer))
			{
				//if the full path is greater than the tmpBuffer(rare!), create new buffer
				std::unique_ptr<char[]> tmpBuf(new char[size + 1]);
				size = GetFullPathNameA(path.c_str(), size + 1, tmpBuf.get(), nullptr);
				if (size)
					return std::string(tmpBuf.get(), size);
			}
			else if (size != 0)
			{
				return std::string(tmpBuffer, size);
			}

			return path;
		}

		//================================================================
		// change current process directory
		//================================================================
		bool changeCurrentDir(const std::string &dir)
		{

			return SetCurrentDirectoryA(dir.c_str()) != 0;

		}

		//=========================================================
		// get the current process directory
		//===========================================================
		std::string getCurrentDir()
		{
			DWORD size = GetCurrentDirectory(sizeof(tmpBuffer), tmpBuffer);
			if (size >= sizeof(tmpBuffer))
			{
				std::unique_ptr<char[]> tmpBuf(new char[size + 1]);
				size = GetCurrentDirectory(size + 1, tmpBuf.get());
				if (size)
					return addSeparator(std::string(tmpBuf.get(), size));
			}
			else if (size != 0)
			{
				return addSeparator(std::string(tmpBuffer, size));
			}

			// Native API failed us. Not ideal.
			return std::string();
		}

		//========================================================
		// create a directory
		//=========================================================
		bool createDirectory(const std::string& path)
		{
			std::string p(stripSeparator(path));

			if (doesFileExist(p))
			{
				return true;
			}
			else
			{
				std::string parent = extractParent(p);
				return CreateDirectory(parent.c_str(), nullptr) != 0;
			}

		}
		*/
	}
}