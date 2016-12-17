#pragma once
#ifndef _FILEUTIL_
#define _FILEUTIL_

#include "core_decls.h"
#include <string>

//===================================================================================
// code sourced from "tunabrain" tungsten raytracer
// https://github.com/tunabrain/tungsten/blob/master/src/core/io/FileUtils.cpp
//===================================================================================

namespace smalltime
{	
namespace fileutil
{

	bool IsSeparator(char c);
	std::string StripSeparator(std::string s);
	std::string AddSeparator(std::string s);

	std::string StripExt(std::string s);
	std::string StripParent(std::string s);

	bool IsRootDirectory(const std::string& s);
	bool IsRelativePath(const std::string& filePath);
//	bool doesfileExist(const std::string& path);


	std::string::size_type FindFilenamePos(const std::string &s);
	std::string ExtractFileExt(std::string s);
	std::string ExtractParent(std::string s);
	//no file ext or parent
	std::string ExtractBase(std::string s);

//	std::string toAbsolutePath(const std::string& path);
//	std::string getCurrentDir();
//	bool changeCurrentDir(const std::string& dir);
//	bool createDirectory(const std::string& path);


}
}

#endif