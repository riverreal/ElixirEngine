#pragma once

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <locale>
#include <codecvt>
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <fstream>

//Releases COM Pointers
template<typename T>
inline void ReleaseCOM(T& pCOM)
{
	if (pCOM != 0)
	{
		pCOM->Release();
		pCOM = 0;
	}
}

//Releases a general pointer
template<typename T>
inline void SafeRelease(T& pOb)
{
	if (pOb != 0)
	{
		delete pOb;
		pOb = 0;
	}
}

//Converts a wstr into a str
static std::string wstrtostr(const std::wstring &wstr)
{
	std::string strTo;
	char *szTo = new char[wstr.length() + 1];
	szTo[wstr.size()] = '\0';
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
	strTo = szTo;
	delete[] szTo;
	return strTo;
}

static std::wstring strtowstr(const std::string &str)
{
	int slength = (int)str.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, &r[0], len);
	return r;
}

static std::wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

static std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}


const std::string LOG_DEFAULT = "log";
const std::string LOG_GRAPHICS = "graphicsLog";
const std::string LOG_SCRIPT = "scriptLog";
const std::string LOG_WARNING = "warningLog";

class Log
{
public:
	Log()
		:m_filename(LOG_DEFAULT + ".log")
	{}

	//Log name without .log termination
	Log(const std::string & p_filename)
		: m_filename(p_filename + ".log")
	{}

	virtual ~Log()
	{
		// implement  your writeToFile() with std::ofstream 
		writeToFile(m_filename, true);
	}

	template< typename T >
	Log & operator<<(const T & p_value)
	{
		m_stream << p_value;
		return *this;
	}

	void writeToFile(std::string filename, bool idk)
	{
		std::ofstream file(m_filename, std::ios::app);
		if (file.is_open())
		{
			file << m_stream.str();
			file.close();
		}
	}

private:
	std::string         m_filename;
	std::ostringstream  m_stream;
};

static void ElixirLog(std::string log)
{
	Log() << log << "\n";
}

//Obsolete
static void RadixLog(std::string log)
{
	Log(LOG_WARNING) << "OBSOLETE LOG! " << log << "\n";
}