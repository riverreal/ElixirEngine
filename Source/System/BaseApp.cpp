#include "BaseApp.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_baseApp->MsgProc(hwnd, msg, wParam, lParam);
}

BaseApp::BaseApp(HINSTANCE instance, int width, int height)
	:m_instance(instance),
	m_width(width),
	m_height(height),
	m_appPaused(false),
	m_appName(L"Testing")
{

}

BaseApp::~BaseApp()
{
	//D3D Shutdown


	//Window Shutdown
	ChangeDisplaySettings(NULL, 0);
	DestroyWindow(m_hWnd);
	UnregisterClass(m_appName, m_instance);
}

bool BaseApp::Init()
{
	if (!InitWindow())
	{
		return false;
	}

	if (!InitD3D())
	{
		return false;
	}

	return true;
}

void BaseApp::Run()
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!m_appPaused)
			{
				//do stuff
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

bool BaseApp::InitWindow()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	g_baseApp = this;

	//m_appName = L"Testing";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_appName;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	int totalWidth = GetSystemMetrics(SM_CXSCREEN);
	int totalHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)m_width;
		dmScreenSettings.dmPelsHeight = (unsigned long)m_height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = posY = 0;
	}
	else
	{
		posX = (totalWidth - m_width) / 2;
		posY = (totalHeight - m_height) / 2;
	}

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, WS_CLIPSIBLINGS, posX, posY, m_width, m_height, NULL, NULL, m_instance, NULL);
	if (!m_hWnd)
	{
		MessageBox(0, L"Failed Creating Window", L"ERROR", MB_OK);
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	return true;
}

bool BaseApp::InitD3D()
{
	return true;
}

LRESULT BaseApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_appPaused = true;
		}
		else
		{
			m_appPaused = false;
		}
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 300;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 300;
		return 0;
	}


	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}