#include <windows.h>

#include "PrismSurface/Error.h"

#include "WindowsTheme.h"

namespace PrismSurface
{
	void InitDarkModeOrdinals()
	{
		HMODULE uxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (!uxtheme)
		{
			ErrorHandler::Error(ErrorCode::ModuleLoadingFailed, "Failed to load uxtheme.dll!");
			return;
		}

		SetPreferredAppMode = reinterpret_cast<fnSetPreferredAppMode>(GetProcAddress(uxtheme, MAKEINTRESOURCEA(135)));
		FlushMenuThemes = reinterpret_cast<fnFlushMenuThemes>(GetProcAddress(uxtheme, MAKEINTRESOURCEA(136)));
		AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(uxtheme, MAKEINTRESOURCEA(133)));
	}

	bool SystemUsesLightTheme()
	{
		DWORD value = 1;
		DWORD size = sizeof(value);
		LONG result = RegGetValueW(
			HKEY_CURRENT_USER,
			L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
			L"AppsUseLightTheme",
			RRF_RT_REG_DWORD, nullptr, &value, &size);

		return (result != ERROR_SUCCESS) || (value != 0);
	}

	bool SystemShowsAccentOnCaption()
	{
		DWORD value = 0;
		DWORD size = sizeof(value);
		LONG result = RegGetValueW(
			HKEY_CURRENT_USER,
			L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
			L"ColorPrevalence",
			RRF_RT_REG_DWORD, nullptr, &value, &size);

		return (result == ERROR_SUCCESS) && (value != 0);
	}

	COLORREF GetSystemAccentColor()
	{
		DWORD color = 0;
		BOOL opaqueBlend = FALSE;
		HRESULT hr = DwmGetColorizationColor(&color, &opaqueBlend);

		if (FAILED(hr))
			return RGB(0, 120, 215);

		BYTE r = (color >> 16) & 0xFF;
		BYTE g = (color >> 8) & 0xFF;
		BYTE b = color & 0xFF;

		return RGB(r, g, b);
	}

	winrt::Windows::UI::Color GetAccentColorWinRT()
	{
		using namespace winrt::Windows::UI::ViewManagement;
		UISettings settings;
		return settings.GetColorValue(UIColorType::Accent);
	}
}
