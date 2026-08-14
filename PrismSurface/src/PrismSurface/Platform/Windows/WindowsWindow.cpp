#include "pch.h"
#if defined(PRISM_PLATFORM_WINDOWS)

#include <windowsx.h>

#include "WindowsWindow.h"
#include "WindowsUtils.h"

namespace PrismSurface
{
	using WindowList = std::vector<WindowsWindow*>;

	static int s_WindowCount = 0;
	static WindowList s_WindowList;

	static const wchar_t* s_WindowClassName = L"PrismSurfaceWindowClass";

	Window* Window::Create(const WindowProperties& properties)
	{
		return new WindowsWindow(properties);
	}

	static DWORD GetStyle(const WindowProperties& properties)
	{
		DWORD style = WS_OVERLAPPEDWINDOW;

		if (!properties.Resizable)
			style &= ~WS_THICKFRAME;
		if (!properties.DefaultTitleBar)
			style &= ~WS_CAPTION;

		return style;
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_CLOSE:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					WindowClosedEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_SIZE:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (!window)
					return 0;

				window->m_Properties.Width = LOWORD(lParam);
				window->m_Properties.Height = HIWORD(lParam);

				if (window->m_Properties.EventCallback)
				{
					WindowResizedEvent event(window->m_Properties.Width, window->m_Properties.Height);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_SETFOCUS:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					WindowFocusedEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_KILLFOCUS:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					WindowLostFocusEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_MOVE:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (!window)
					return 0;

				window->m_Properties.Position.first = LOWORD(lParam);
				window->m_Properties.Position.second = HIWORD(lParam);

				if (window->m_Properties.EventCallback)
				{
					WindowMovedEvent event(window->m_Properties.Position.first, window->m_Properties.Position.second);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_KEYDOWN:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					Key key = static_cast<Key>(wParam);
					KeyPressedEvent event(key);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_KEYUP:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					Key key = static_cast<Key>(wParam);
					KeyReleasedEvent event(key);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_CHAR:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					Key key = static_cast<Key>(wParam);
					KeyTypedEvent event(key);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_XBUTTONDOWN:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					MouseButton button = GetButtonFromMessage(msg, wParam);
					MouseButtonPressedEvent event(button);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					MouseButton button = GetButtonFromMessage(msg, wParam);
					MouseButtonReleasedEvent event(button);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_MOUSEMOVE:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					float xPos = static_cast<float>(GET_X_LPARAM(lParam));
					float yPos = static_cast<float>(GET_Y_LPARAM(lParam));

					MouseMovedEvent event(xPos, yPos);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_MOUSEWHEEL:
			case WM_MOUSEHWHEEL:
			{
				WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
				if (window && window->m_Properties.EventCallback)
				{
					float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
					MouseScrolledEvent event(delta, msg == WM_MOUSEHWHEEL);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

		}

		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(const WindowProperties& properties) : Window(properties)
	{
		m_InstanceHandle = GetModuleHandle(nullptr);

		if (s_WindowCount == 0)
		{
			WNDCLASSEXW wndClass{};
			wndClass.lpszClassName = s_WindowClassName;
			wndClass.cbSize = sizeof(wndClass);
			wndClass.hInstance = m_InstanceHandle;
			// Reinterpret cast to avoid explicit UNICODE definition
			wndClass.hIcon = LoadIconW(NULL, reinterpret_cast<LPCWSTR>(IDI_WINLOGO));
			wndClass.hCursor = LoadCursorW(NULL, reinterpret_cast<LPCWSTR>(IDC_ARROW));
			wndClass.lpfnWndProc = WindowProc;

			RegisterClassExW(&wndClass);
		}

		s_WindowCount++;
		s_WindowList.push_back(this);

		RECT windowRect;

		windowRect.left = m_Properties.Centered ? (GetSystemMetrics(SM_CXSCREEN) - m_Properties.Width) / 2 : m_Properties.Position.first;
		windowRect.top = m_Properties.Centered ? (GetSystemMetrics(SM_CYSCREEN) - m_Properties.Height) / 2 : m_Properties.Position.second;
		windowRect.right = windowRect.left + m_Properties.Width;
		windowRect.bottom = windowRect.top + m_Properties.Height;

		DWORD style = GetStyle(m_Properties);
		AdjustWindowRect(&windowRect, style, false);

		std::wstring title = std::wstring(m_Properties.Title.begin(), m_Properties.Title.end());
		m_WindowHandle = CreateWindowExW(0,
										s_WindowClassName,
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

		SetWindowLongPtrW(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		if (m_Properties.Visible)
			Show();
	}

	WindowsWindow::~WindowsWindow()
	{
		DestroyWindow(m_WindowHandle);

		s_WindowList.erase(std::remove(s_WindowList.begin(), s_WindowList.end(), this), s_WindowList.end());
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
			if (msg.message == WM_QUIT)
			{
				// Copy window list to avoid modifying it while iterating
				WindowList windowsToClose = s_WindowList;

				for (WindowsWindow* window : windowsToClose)
					window->Close();

				return;
			}

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
		PostMessage(m_WindowHandle, WM_CLOSE, 0, 0);
	}
}

#endif
