#include "pch.h"
#if defined(PRISM_PLATFORM_WINDOWS)

#include "WindowsWindow.h"

extern bool isRunning;

namespace PrismSurface
{
	static const wchar_t* s_WindowClassName = L"PrismSurfaceWindowClass";
	static int s_WindowCount = 0;

	Window* Window::Create(const WindowProperties& properties)
	{
		return new WindowsWindow(properties);
	}

	static DWORD GetWindowStyle(const WindowProperties& properties)
	{
		DWORD style = WS_OVERLAPPEDWINDOW;

		if (!properties.Resizable)
			style &= ~WS_THICKFRAME;
		if (!properties.DefaultTitleBar)
			style &= ~WS_CAPTION;

		return style;
	}

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			isRunning = false;
			DestroyWindow(hwnd);
			return 0;
		}

		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& properties) : Window(properties)
	{
		s_WindowCount++;
		m_InstanceHandle = GetModuleHandle(nullptr);

		WNDCLASSEXW wndClass{};
		wndClass.lpszClassName = s_WindowClassName;
		wndClass.cbSize = sizeof(wndClass);
		wndClass.hInstance = m_InstanceHandle;
		// Reinterpret cast to avoid explicit UNICODE definition
		wndClass.hIcon = LoadIconW(NULL, reinterpret_cast<LPCWSTR>(IDI_WINLOGO));
		wndClass.hCursor = LoadCursorW(NULL, reinterpret_cast<LPCWSTR>(IDC_ARROW));
		wndClass.lpfnWndProc = WindowProc;

		RegisterClassExW(&wndClass);

		RECT windowRect;

		windowRect.left = m_Properties.Centered ? (GetSystemMetrics(SM_CXSCREEN) - m_Properties.Width) / 2 : m_Properties.Position.first;
		windowRect.top = m_Properties.Centered ? (GetSystemMetrics(SM_CYSCREEN) - m_Properties.Height) / 2 : m_Properties.Position.second;
		windowRect.right = windowRect.left + m_Properties.Width;
		windowRect.bottom = windowRect.top + m_Properties.Height;

		DWORD style = GetWindowStyle(m_Properties);
		AdjustWindowRect(&windowRect, style, false);

		SetWindowLongPtr(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		std::wstring title = std::wstring(m_Properties.Title.begin(), m_Properties.Title.end());
		m_WindowHandle = CreateWindowExW(0,
										wndClass.lpszClassName,
										title.c_str(),
										style,
										windowRect.left,
										windowRect.top,
										windowRect.right - windowRect.left,
										windowRect.bottom - windowRect.top,
										NULL,
										NULL,
										m_InstanceHandle,
										NULL
										);

		if (m_Properties.Visible)
			Show();
	}

	WindowsWindow::~WindowsWindow()
	{
		if (--s_WindowCount == 0)
			UnregisterClassW(s_WindowClassName, m_InstanceHandle);
	}

	const void* WindowsWindow::GetNativeWindow() const
	{
		return GetWindowHandle();
	}

	void WindowsWindow::Update()
	{
		MSG msg;

		while (PeekMessage(&msg, m_WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::Resize(uint32_t width, uint32_t height)
	{
		m_Properties.Width = width;
		m_Properties.Height = height;
	}

	void WindowsWindow::SetFullscreen()
	{
		m_Properties.CurrentState = WindowState::Fullscreen;
		// TODO
	}

	void WindowsWindow::Minimize()
	{
		m_Properties.CurrentState = WindowState::Minimized;
		ShowWindow(m_WindowHandle, SW_MINIMIZE);
	}

	void WindowsWindow::Maximize()
	{
		m_Properties.CurrentState = WindowState::Maximized;
		ShowWindow(m_WindowHandle, SW_MAXIMIZE);
	}

	void WindowsWindow::Restore()
	{
		m_Properties.CurrentState = WindowState::Normal;
		ShowWindow(m_WindowHandle, SW_RESTORE);
	}

	void WindowsWindow::Show()
	{
		ShowWindow(m_WindowHandle, SW_SHOW);
	}

	void WindowsWindow::Hide()
	{
		ShowWindow(m_WindowHandle, SW_HIDE);
	}

	void WindowsWindow::Close()
	{
		CloseWindow(m_WindowHandle);
	}
}

#endif
