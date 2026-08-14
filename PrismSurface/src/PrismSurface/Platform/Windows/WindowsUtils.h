#pragma once

#include <windows.h>
#include <windowsx.h>

#include "PrismSurface/PrismSurface.h"

#include "PrismSurface/KeyCodes.h"

namespace PrismSurface
{
	PRISM_SOURCE_API inline MouseButton GetButtonFromMessage(UINT msg, WPARAM wParam)
	{
		switch (msg)
		{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
				return MouseButton::Left;

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
				return MouseButton::Right;

			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
				return MouseButton::Middle;

			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
				return (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
					? MouseButton::Button4 : MouseButton::Button5;
			default:
				return MouseButton::Unknown;
		}
	}
}
