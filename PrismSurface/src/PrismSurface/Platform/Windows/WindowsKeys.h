#pragma once

#include <windows.h>

#include "PrismSurface/PrismSurfaceConfig.h"
#include "PrismSurface/KeyCodes.h"

namespace PrismSurface
{
	Key PRISM_SOURCE_API GetKeyFromWinKeyCode(WPARAM wParam, LPARAM lParam);
	MouseButton PRISM_SOURCE_API GetButtonFromMessage(UINT msg, WPARAM wParam);
}
