#pragma once

#include <dwmapi.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <Uxtheme.h>

#include "PrismSurface/PrismSurfaceConfig.h"

namespace PrismSurface
{
	using fnSetPreferredAppMode = int(WINAPI*)(int); // ordinal 135 (1903+)
	using fnFlushMenuThemes = void(WINAPI*)();        // ordinal 136
	using fnAllowDarkModeForWindow = bool(WINAPI*)(HWND, bool); // ordinal 133

	enum PreferredAppMode { Default = 0, AllowDark = 1, ForceDark = 2, ForceLight = 3, Max = 4 };

	inline fnSetPreferredAppMode SetPreferredAppMode = nullptr;
	inline fnFlushMenuThemes FlushMenuThemes = nullptr;
	inline fnAllowDarkModeForWindow AllowDarkModeForWindow = nullptr;

	PRISM_SOURCE_API void InitDarkModeOrdinals();

	PRISM_SOURCE_API bool SystemUsesLightTheme();
	PRISM_SOURCE_API bool SystemShowsAccentOnCaption();
	PRISM_SOURCE_API COLORREF GetSystemAccentColor();
	PRISM_SOURCE_API winrt::Windows::UI::Color GetAccentColorWinRT();
}
