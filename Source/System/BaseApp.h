#pragma once

#define WIN32_LEAN_AND_MEAN

const bool FULL_SCREEN = false;

#include <Windows.h>

class BaseApp
{
public:
	BaseApp(HINSTANCE instance, int width, int height);
	virtual ~BaseApp();

	virtual bool Init();
	void Run();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	bool InitWindow();
	bool InitD3D();

private:
	HINSTANCE m_instance;
	HWND m_hWnd;

	int m_width;
	int m_height;

	LPCWSTR m_appName;

	bool m_resizing;
	bool m_appPaused;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BaseApp* g_baseApp = 0;