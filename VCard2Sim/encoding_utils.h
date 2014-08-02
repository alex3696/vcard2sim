#ifndef __ENCODING_UTILS_H
#define __ENCODING_UTILS_H

#include "stdafx.h"
#include <windows.h>


//boost::spirit::utf8_string strng  почтать, может тут чего есть полезного!!!


//-----------------------------------------------------------------------------
std::string wchar2char(const std::wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}
//-----------------------------------------------------------------------------
// Convert an UTF8 string to a wide Unicode String
std::wstring char2wchar(const std::string &str)
{
	int size_needed;
	if ((unsigned char)str[0] == 0xEF && (unsigned char)str[1] == 0xBB && (unsigned char)str[2] == 0xBF) //detect BOM
	{
		size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[3], (int)str.size() - 3, NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[3], (int)str.size() - 3, &wstrTo[0], size_needed);
		return wstrTo;
	}
	else
	{
		size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}
}
//-----------------------------------------------------------------------------
size_t GetSizeOfFile(const std::wstring& path)
{
	struct _stat fileinfo;
	_wstat(path.c_str(), &fileinfo);
	return fileinfo.st_size;
}
//-----------------------------------------------------------------------------
std::wstring LoadUtf8FileToString(const std::wstring& filename)
{
	std::wstring buffer;            // stores file contents

	FILE* f(nullptr);
	errno_t err = _wfopen_s(&f, filename.c_str(), L"rtS, ccs=UTF-8");

	// Failed to open file
	if (f == NULL)
	{
		// ...handle some error...
		return buffer;
	}

	size_t filesize = GetSizeOfFile(filename);

	// Read entire file contents in to memory
	if (filesize > 0)
	{
		buffer.resize(filesize);
		size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
		buffer.resize(wchars_read);
		buffer.shrink_to_fit();
	}

	fclose(f);

	return buffer;
}
//-----------------------------------------------------------------------------
// http://www.codeproject.com/Articles/38242/Reading-UTF-with-C-streams
void ReadUtf8File(const std::wstring &fileName, std::wstring &result)
{
	//const std::locale empty_locale = std::locale::empty();
	//typedef std::codecvt_utf8<wchar_t> converter_type;
	//const converter_type* converter = new converter_type;
	//const std::locale utf8_locale = std::locale(empty_locale, converter);

	std::wifstream in;
	//in.imbue(utf8_locale);
	in.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

	in.open(fileName, std::wifstream::in);
	if (in.is_open())
	{
		//	std::wstring buf;
		//	while (std::getline(in, buf))
		//		result += buf + L"\n";
		std::wstringstream wss;
		wss << in.rdbuf();
		result = wss.str();

	}

	in.close();

}
//-----------------------------------------------------------------------------


#endif //