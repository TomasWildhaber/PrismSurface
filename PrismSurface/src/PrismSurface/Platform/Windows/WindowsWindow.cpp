#include "pch.h"
#if defined(PRISM_PLATFORM_WINDOWS)

#include <windowsx.h>
#include <dwmapi.h>

#include "WindowsWindow.h"
#include "WindowsUtils.h"
#include "WindowsTheme.h"

namespace PrismSurface
{
	using WindowList = std::vector<WindowsWindow*>;

	static WindowList s_WindowList;
	static const wchar_t* s_WindowClassName = L"PrismSurfaceWindowClass";

	Window* Window::Create(const WindowProperties& properties)
	{
		return new WindowsWindow(properties);
	}

	static DWORD GetStyle(const WindowProperties& properties)
	{
		DWORD style = properties.Frame ? WS_OVERLAPPEDWINDOW : WS_POPUP;

		if (!properties.Resizable)
				style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;

		return style;
	}

	static POINT GetBorderSize()
	{
		POINT borderSize;

		borderSize.x = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
		borderSize.y = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

		return borderSize;
	}

	static WindowsWindow* GetWindowFromHandle(HWND hwnd)
	{
		return reinterpret_cast<WindowsWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_NCCREATE:
			{
				CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
				WindowsWindow* window = static_cast<WindowsWindow*>(createStruct->lpCreateParams);
				SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

				break;
			}

			case WM_NCACTIVATE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if (!window->HasFrame() || !window->HasDefaultTitleBar())
					return TRUE;

				break;
			}

			case WM_CREATE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if (!window->HasDefaultTitleBar() || !window->HasFrame())
				{
					SetWindowPos(
						hwnd, NULL,
						0, 0, 0, 0,
						SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
					);
				}

				return 0;
			}

			case WM_NCCALCSIZE:
			{
				if (wParam != TRUE)
					break;

				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if (!window->HasFrame())
					return 0;

				if (!window->HasDefaultTitleBar())
				{
					POINT borderSize = GetBorderSize();

					NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
					RECT* clientRect = params->rgrc;

					clientRect->right -= borderSize.x;
					clientRect->left += borderSize.x;
					clientRect->bottom -= borderSize.y;

					return 0;
				}

				break;
			}

			case WM_ERASEBKGND:
			{
				return TRUE;
			}

			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				RECT clientRect;
				GetClientRect(hwnd, &clientRect);

				// This is your solid client area. DWM will NOT make this transparent
				// because it is not painted pure black (0,0,0).
				HBRUSH hBlueBrush = CreateSolidBrush(RGB(0, 0, 255));
				FillRect(hdc, &clientRect, hBlueBrush);
				DeleteObject(hBlueBrush);

				EndPaint(hwnd, &ps);

				return 0;
			}

			case WM_NCHITTEST:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (!window || !window->HasFrame())
					return HTNOWHERE;

				if (window->HasDefaultTitleBar())
					break;

				POINT cursorPos;
				cursorPos.x = GET_X_LPARAM(lParam);
				cursorPos.y = GET_Y_LPARAM(lParam);
				ScreenToClient(hwnd, &cursorPos);

				bool maximized = IsZoomed(hwnd);

				if (window->IsResizable() && !maximized)
				{
					RECT clientRect;
					GetClientRect(hwnd, &clientRect);

					enum HitAreas { Left = 1, Top = 2, Right = 4, Bottom = 8 };
					constexpr int topExtend = 2;

					int hit = 0;
					if (cursorPos.x <= clientRect.left)
						hit |= Left;
					if (cursorPos.x >= clientRect.right)
						hit |= Right;
					if (cursorPos.y <= clientRect.top + topExtend)
						hit |= Top;
					if (cursorPos.y >= clientRect.bottom)
						hit |= Bottom;

					if (hit & Top && hit & Left)        return HTTOPLEFT;
					if (hit & Top && hit & Right)       return HTTOPRIGHT;
					if (hit & Bottom && hit & Left)     return HTBOTTOMLEFT;
					if (hit & Bottom && hit & Right)    return HTBOTTOMRIGHT;
					if (hit & Left)                     return HTLEFT;
					if (hit & Top)                      return HTTOP;
					if (hit & Right)                    return HTRIGHT;
					if (hit & Bottom)                   return HTBOTTOM;
				}

				if (window->m_Properties.EventCallback)
				{
					TitlebarHittestEvent::HittestResult titlebarHitttest =
						TitlebarHittestEvent::HittestResult::None;

					TitlebarHittestEvent event(static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y), titlebarHitttest);
					window->m_Properties.EventCallback(event);

					switch (titlebarHitttest)
					{
						case PrismSurface::TitlebarHittestEvent::HittestResult::None: 				return HTCLIENT;
						case PrismSurface::TitlebarHittestEvent::HittestResult::Titlebar:			return HTCAPTION;
						case PrismSurface::TitlebarHittestEvent::HittestResult::MinimizeButton:		return HTMINBUTTON;
						case PrismSurface::TitlebarHittestEvent::HittestResult::MaximizeButton:		return HTMAXBUTTON;
						case PrismSurface::TitlebarHittestEvent::HittestResult::CloseButton:		return HTCLOSE;
					}
				}

				return HTCLIENT;
			}

			case WM_NCLBUTTONDOWN:
			case WM_NCLBUTTONUP:
			{
				bool isInButtons = wParam == HTMINBUTTON || wParam == HTMAXBUTTON || wParam == HTCLOSE;
				if (!isInButtons)
					break;

				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if (!window->HasDefaultTitleBar() && msg == WM_NCLBUTTONUP)
				{
					switch (wParam)
					{
						case HTMINBUTTON:		ShowWindow(hwnd, SW_MINIMIZE); break;
						case HTMAXBUTTON:		if (window->IsResizable()) { ShowWindow(hwnd, SW_MAXIMIZE); } break;
						case HTCLOSE:			PostMessage(hwnd, WM_CLOSE, 0, 0); break;
					}

					return 0;
				}

				if (!window->HasDefaultTitleBar())
					return 0;

				break;
			}

			case WM_NCRBUTTONDOWN:
			case WM_NCRBUTTONUP:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				bool isInTitlebar = (wParam == HTCAPTION || wParam == HTSYSMENU || wParam == HTMINBUTTON || wParam == HTMAXBUTTON);

				if (!window->m_Properties.WinMenu && isInTitlebar)
					return 0;

				break;
			}

			case WM_SETTINGCHANGE:
			{
				if (lParam)
				{
					auto* str = reinterpret_cast<const wchar_t*>(lParam);
					if (wcscmp(str, L"ImmersiveColorSet") == 0)
					{
						WindowsWindow* window = GetWindowFromHandle(hwnd);

						if (window && window->GetTheme() == Theme::System)
							window->SetTheme(Theme::System);
					}
				}

				break;
			}

			case WM_DWMCOLORIZATIONCOLORCHANGED:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if (window && window->GetTheme() == Theme::System)
					window->SetTheme(Theme::System);

				break;
			}

			case WM_SYSCOMMAND:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if ((wParam & 0xFFF0) == SC_KEYMENU && !window->m_Properties.WinMenu)
					return 0;

				break;
			}

			case WM_CLOSE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window && window->m_Properties.EventCallback)
				{
					WindowClosedEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_SIZE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window && window->m_Properties.EventCallback)
				{
					WindowFocusedEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_KILLFOCUS:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window && window->m_Properties.EventCallback)
				{
					WindowLostFocusEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_MOVE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (!window)
					return 0;

				window->m_Properties.Position.first = GET_X_LPARAM(lParam);
				window->m_Properties.Position.second = GET_Y_LPARAM(lParam);

				if (window->m_Properties.EventCallback)
				{
					WindowMovedEvent event(window->m_Properties.Position.first, window->m_Properties.Position.second);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_KEYDOWN:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
				WindowsWindow* window = GetWindowFromHandle(hwnd);
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
		m_WindowHandle = nullptr;

		if (s_WindowList.empty())
		{
			InitDarkModeOrdinals();

			WNDCLASSEXW wndClass{};
			wndClass.cbSize = sizeof(wndClass);
			wndClass.style = CS_HREDRAW | CS_VREDRAW;
			wndClass.lpszClassName = s_WindowClassName;
			wndClass.hInstance = m_InstanceHandle;
			// Reinterpret cast to avoid depending on an explicit UNICODE define;
			wndClass.hIcon = LoadIconW(NULL, reinterpret_cast<LPCWSTR>(IDI_WINLOGO));
			wndClass.hCursor = LoadCursorW(NULL, reinterpret_cast<LPCWSTR>(IDC_ARROW));
			wndClass.lpfnWndProc = WindowProc;

			if (!RegisterClassExW(&wndClass))
			{
				// TODO: Error handling
				return;
			}
		}

		s_WindowList.push_back(this);

		RECT windowRect;

		windowRect.left = m_Properties.Centered ? (GetSystemMetrics(SM_CXSCREEN) - m_Properties.Width) / 2 : m_Properties.Position.first;
		windowRect.top = m_Properties.Centered ? (GetSystemMetrics(SM_CYSCREEN) - m_Properties.Height) / 2 : m_Properties.Position.second;
		windowRect.right = windowRect.left + m_Properties.Width;
		windowRect.bottom = windowRect.top + m_Properties.Height;

		DWORD style = GetStyle(m_Properties);
		AdjustWindowRect(&windowRect, style, FALSE);

		if (!m_Properties.DefaultTitleBar && m_Properties.Frame)
			windowRect.top += GetSystemMetrics(SM_CYCAPTION) + GetBorderSize().y;

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
			this
		);

		if (!m_WindowHandle)
		{
			// TODO: Error handling
			return;
		}

		SetTheme(m_Properties.CurrentTheme);

		switch (m_Properties.CurrentState)
		{
			case WindowState::Normal:			ShowWindow(m_WindowHandle, SW_RESTORE); break;
			case WindowState::Minimized:		ShowWindow(m_WindowHandle, SW_MINIMIZE); break;
			case WindowState::Maximized:		ShowWindow(m_WindowHandle, SW_MAXIMIZE); break;
		}

		if (m_Properties.Visible)
			Show();
	}

	WindowsWindow::~WindowsWindow()
	{
		if (m_WindowHandle)
			DestroyWindow(m_WindowHandle);

		s_WindowList.erase(std::remove(s_WindowList.begin(), s_WindowList.end(), this), s_WindowList.end());
		if (s_WindowList.empty())
			UnregisterClassW(s_WindowClassName, m_InstanceHandle);
	}

	const void* WindowsWindow::GetNativeHandle() const
	{
		return GetHandle();
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

	void WindowsWindow::SetTheme(Theme theme)
	{
		m_Properties.CurrentTheme = theme;

		BOOL dark = FALSE;
		switch (m_Properties.CurrentTheme)
		{
			case Theme::Light:  dark = FALSE; break;
			case Theme::Dark:   dark = TRUE;  break;
			case Theme::System: dark = !SystemUsesLightTheme(); break;
		}

		DwmSetWindowAttribute(m_WindowHandle, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

		if (SetPreferredAppMode)
			SetPreferredAppMode(dark ? AllowDark : Default);
		if (AllowDarkModeForWindow)
			AllowDarkModeForWindow(m_WindowHandle, dark);
		if (FlushMenuThemes)
			FlushMenuThemes();

		if (theme == Theme::System && SystemShowsAccentOnCaption())
		{
			COLORREF accent = GetSystemAccentColor();
			DwmSetWindowAttribute(m_WindowHandle, DWMWA_BORDER_COLOR, &accent, sizeof(accent));
			DwmSetWindowAttribute(m_WindowHandle, DWMWA_CAPTION_COLOR, &accent, sizeof(accent));
		}
		else
		{
			COLORREF defaultColor = DWMWA_COLOR_DEFAULT;
			DwmSetWindowAttribute(m_WindowHandle, DWMWA_BORDER_COLOR, &defaultColor, sizeof(defaultColor));
			DwmSetWindowAttribute(m_WindowHandle, DWMWA_CAPTION_COLOR, &defaultColor, sizeof(defaultColor));
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
