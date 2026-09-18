#include "pch.h"
#if defined(PRISM_PLATFORM_WINDOWS)

#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>

#include "PrismSurface/Error.h"

#include "WindowsWindow.h"
#include "WindowsKeys.h"
#include "WindowsTheme.h"

namespace PrismSurface
{
	using WindowList = std::vector<WindowsWindow*>;

	static WindowList s_WindowList;
	static const wchar_t* s_WindowClassName = L"PrismSurfaceWindowClass";

	static DWORD GetStyle(const WindowProperties& properties)
	{
		DWORD style = properties.Frame ? WS_OVERLAPPEDWINDOW : WS_POPUP;

		if (!properties.Resizable)
				style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;

		return style;
	}

	static POINT GetWindowBorderSize(HWND hwnd)
	{
		POINT borderSize;
		UINT dpi = GetDpiForWindow(hwnd);

		borderSize.x = GetSystemMetricsForDpi(SM_CXFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
		borderSize.y = GetSystemMetricsForDpi(SM_CYFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

		return borderSize;
	}
	
	static POINT GetSystemBorderSize()
	{
		POINT borderSize;
		UINT dpi = GetDpiForSystem();

		borderSize.x = GetSystemMetricsForDpi(SM_CXFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
		borderSize.y = GetSystemMetricsForDpi(SM_CYFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

		return borderSize;
	}

	static WindowsWindow* GetWindowFromHandle(HWND hwnd)
	{
		return reinterpret_cast<WindowsWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	Window* Window::Create(const WindowProperties& properties)
	{
		WindowsWindow* window = new WindowsWindow(properties);

		if (!window->GetHandle())
		{
			delete window;
			return nullptr;
		}

		return window;
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

				if (window->m_Properties.DragAndDrop)
					DragAcceptFiles(hwnd, TRUE);

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
					POINT borderSize = GetWindowBorderSize(hwnd);

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

				if (!window->HasFrame())
					return HTCLIENT;

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

			case WM_GETMINMAXINFO:
			{
				// The order of messages causes WM_GETMINMAXINFO to be sent before WM_CREATE,
				// therefore this is the only case when window can be nullptr
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (!window)
					break;

				const WindowSize& windowSize = window->m_Properties.Size;
				MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

				// Set the minimum size
				if (windowSize.HasMinWidth())
					mmi->ptMinTrackSize.x = windowSize.MinWidth;

				if (windowSize.HasMinHeight())
					mmi->ptMinTrackSize.y = windowSize.MinHeight;

				// Set the maximum size
				if (windowSize.HasMaxWidth())
					mmi->ptMaxTrackSize.x = windowSize.MaxWidth;

				if (windowSize.HasMaxHeight())
					mmi->ptMaxTrackSize.y = windowSize.MaxHeight;

				return 0;
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

			case WM_DPICHANGED:
			{
				RECT* newRect = reinterpret_cast<RECT*>(lParam);

				SetWindowPos(hwnd, nullptr,
							 newRect->left,
							 newRect->top,
							 newRect->right - newRect->left,
							 newRect->bottom - newRect->top,
							 SWP_NOZORDER | SWP_NOACTIVATE);

				return 0;
			}

			case WM_DROPFILES:
			{
				HDROP hDrop = reinterpret_cast<HDROP>(wParam);

				UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
				char** paths = new char*[fileCount];

				for (UINT i = 0; i < fileCount; i++)
				{
					UINT nameLenght = DragQueryFileW(hDrop, i, nullptr, 0);

					if (nameLenght > 0)
					{
						// Temp WCHAR buffer
						WCHAR* wPath = new WCHAR[nameLenght + 1];
						DragQueryFileW(hDrop, i, wPath, nameLenght + 1);

						int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wPath, -1, nullptr, 0, nullptr, nullptr);

						if (utf8Size > 0)
						{
							paths[i] = new char[utf8Size];
							WideCharToMultiByte(CP_UTF8, 0, wPath, -1, paths[i], utf8Size, nullptr, nullptr);
						}
						else
						{
							paths[i] = new char[1]{ '\0' };
						}

						delete[] wPath;
					}
				}

				DragFinish(hDrop);

				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
				{
					DragFileDroppedEvent event(paths, fileCount, static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
					window->m_Properties.EventCallback(event);
				}

				for (UINT i = 0; i < fileCount; i++)
					delete[] paths[i];

				delete[] paths;

				return 0;
			}

			case WM_SYSCOMMAND:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if ((wParam & 0xFFF0) == SC_KEYMENU && !window->m_Properties.WinMenu)
					return 0;

				break;
			}

			case WM_SYSCHAR:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				if (!window->m_Properties.WinMenu)
					return 0;

				break;
			}

			case WM_CLOSE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
				{
					WindowClosedEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_SIZE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				window->m_Properties.Size.Width = LOWORD(lParam);
				window->m_Properties.Size.Height = HIWORD(lParam);

				if (window->m_Properties.EventCallback)
				{
					WindowResizedEvent event(window->m_Properties.Size.Width, window->m_Properties.Size.Height);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_SETFOCUS:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
				{
					WindowFocusedEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_KILLFOCUS:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
				{
					WindowLostFocusEvent event;
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_MOVE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				window->m_Properties.Position.X = GET_X_LPARAM(lParam);
				window->m_Properties.Position.Y = GET_Y_LPARAM(lParam);

				if (window->m_Properties.EventCallback)
				{
					WindowMovedEvent event(window->m_Properties.Position.X, window->m_Properties.Position.Y);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_CHAR:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
				{
					uint32_t charCode = static_cast<uint32_t>(wParam);
					KeyTypedEvent event(charCode);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_UNICHAR:
			{
				if (wParam == UNICODE_NOCHAR)
					return TRUE;

				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
				{
					uint32_t charCode = static_cast<uint32_t>(wParam);
					KeyTypedEvent event(charCode);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);

				bool sendCallback = true;
				bool handled = true;

				Key key = GetKeyFromWinKeyCode(wParam, lParam);
				bool isPressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
				bool isRepeated = (HIWORD(lParam) & KF_REPEAT) != 0;

				switch (wParam)
				{
					case VK_CONTROL:
					{
						MSG nextMsg;
						DWORD time = GetMessageTime();

						// Ignore this control message when next message is right alt
						// Meaning key that was pressed is right alt
						if (PeekMessage(&nextMsg, hwnd, 0, 0, PM_NOREMOVE)) {

							bool isKeyEvent = (nextMsg.message == WM_KEYDOWN || nextMsg.message == WM_SYSKEYDOWN ||
								nextMsg.message == WM_KEYUP || nextMsg.message == WM_SYSKEYUP);

							bool isRightAlt = (nextMsg.wParam == VK_MENU) && ((HIWORD(nextMsg.lParam) & KF_EXTENDED) != 0);

							if (isKeyEvent && isRightAlt && nextMsg.time == time)
								sendCallback = false;
						}

						break;
					}

					case VK_F4:
					{
						// Control alt+f4 behavior
						bool isAltDown = (HIWORD(lParam) & KF_ALTDOWN) != 0;

						if (isAltDown && window->m_Properties.AltF4Close)
						{
							handled = false;
							sendCallback = false;
						}

						break;
					}
				}

				if (sendCallback && window->m_Properties.EventCallback)
				{
					if (isPressed)
					{
						if (!isRepeated)
						{
							KeyPressedEvent event(key);
							window->m_Properties.EventCallback(event);
						}
					}
					else
					{
						KeyReleasedEvent event(key);
						window->m_Properties.EventCallback(event);
					}
				}

				if (handled)
					return 0;
				else
					break;
			}

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_XBUTTONDOWN:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
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
				if (window->m_Properties.EventCallback)
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

				// Windows api does not have WM_MOUSEENTER message
				// so we do it manually
				bool wasMouseTracked = window->m_MouseTracked;
				if (!wasMouseTracked)
				{
					TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hwnd;
					TrackMouseEvent(&tme);

					window->m_MouseTracked = true;
				}

				if (window->m_Properties.EventCallback)
				{
					if (!wasMouseTracked)
					{
						MouseEnteredEvent event;
						window->m_Properties.EventCallback(event);
					}

					float xPos = static_cast<float>(GET_X_LPARAM(lParam));
					float yPos = static_cast<float>(GET_Y_LPARAM(lParam));

					MouseMovedEvent event(xPos, yPos);
					window->m_Properties.EventCallback(event);
				}

				return 0;
			}

			case WM_MOUSELEAVE:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
				{
					MouseLeftEvent event;
					window->m_Properties.EventCallback(event);
				}

				window->m_MouseTracked = false;

				return 0;
			}

			case WM_MOUSEWHEEL:
			case WM_MOUSEHWHEEL:
			{
				WindowsWindow* window = GetWindowFromHandle(hwnd);
				if (window->m_Properties.EventCallback)
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
				ErrorHandler::Error(ErrorCode::WindowCreationFailed, "Failed to create window!");
				return;
			}

			if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
				SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
		}

		s_WindowList.push_back(this);

		UINT dpi = GetDpiForSystem();

		// For windows 96 means default 100% scaling, so it acts as a scaling factor for the window size.
		constexpr int scalingFactor = 96;

		int scaledWidth = MulDiv(m_Properties.Size.Width, dpi, scalingFactor);
		int scaledHeight = MulDiv(m_Properties.Size.Height, dpi, scalingFactor);

		RECT windowRect{ 0, 0, scaledWidth, scaledHeight };

		DWORD style = GetStyle(m_Properties);
		AdjustWindowRectExForDpi(&windowRect, style, FALSE, 0, dpi);

		// Cannot use GetWindowBorderSize here, cause we don't have valid window handle yet.
		if (!m_Properties.DefaultTitleBar && m_Properties.Frame)
			windowRect.top += GetSystemMetricsForDpi(SM_CYCAPTION, dpi) + GetSystemBorderSize().y;

		int windowX = CW_USEDEFAULT;
		int windowY = CW_USEDEFAULT;
		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		bool hasPosition = m_Properties.Position != WindowPosition::AnyPosition();
		bool isCentered = m_Properties.Position == WindowPosition::Centered();

		if (isCentered)
		{
			windowX = (GetSystemMetricsForDpi(SM_CXSCREEN, dpi) - scaledWidth) / 2;
			windowY = (GetSystemMetricsForDpi(SM_CYSCREEN, dpi) - scaledHeight) / 2;
		}
		else if (hasPosition)
		{
			windowX = m_Properties.Position.X;
			windowY = m_Properties.Position.Y;
		}

		std::wstring title = std::wstring(m_Properties.Title.begin(), m_Properties.Title.end());
		m_WindowHandle = CreateWindowExW(
			0,
			s_WindowClassName,
			title.c_str(),
			style,
			windowX,
			windowY,
			windowWidth,
			windowHeight,
			NULL,
			NULL,
			m_InstanceHandle,
			this
		);

		if (!m_WindowHandle)
		{
			ErrorHandler::Error(ErrorCode::WindowCreationFailed, "Failed to create window!");
			return;
		}

		if (isCentered || !hasPosition)
		{
			RECT rect;
			GetWindowRect(m_WindowHandle, &rect);

			m_Properties.Position.X = rect.left;
			m_Properties.Position.Y = rect.top;
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

		while (PeekMessageW(&msg, m_WindowHandle, 0, 0, PM_REMOVE))
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
			DispatchMessageW(&msg);
		}
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		m_Properties.Title = title;
		std::wstring wideTitle = std::wstring(title.begin(), title.end());

		SetWindowTextW(m_WindowHandle, wideTitle.c_str());
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
		if (!m_Properties.DefaultTitleBar && m_Properties.Frame)
		{
			// Adjust the size to account for window borders and title bar
			// X borders are left and right, so we multiply by 2
			// Y border is only the bottom, so we add it once
			width += GetWindowBorderSize(m_WindowHandle).x * 2;
			height += GetWindowBorderSize(m_WindowHandle).y;
		}

		SetWindowPos(m_WindowHandle, NULL, NULL, NULL, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
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
