#pragma once

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <iostream>
#include <Windows.h>

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

static void RadixLog(std::string log)
{
	std::cout << log.c_str() << std::endl;
}