#include "WindowsKeys.h"

// Bitmask for key scancode in LPARAM of WM_KEYDOWN, WM_SYSKEYDOWN, WM_KEYUP and WM_SYSKEYUP messages
#ifndef KF_SCANCODE
#define KF_SCANCODE 0x00FF
#endif

namespace PrismSurface
{
	static Key GetKeyFromScancode(LPARAM lParam)
	{
		UINT scancode = (HIWORD(lParam) & (KF_EXTENDED | KF_SCANCODE));

		switch (scancode)
		{
			case 0x10:	return Key::Q;
			case 0x11:	return Key::W;
			case 0x12:	return Key::E;
			case 0x13:	return Key::R;
			case 0x14:	return Key::T;
			case 0x15:	return Key::Y;
			case 0x16:	return Key::U;
			case 0x17:	return Key::I;
			case 0x18:	return Key::O;
			case 0x19:	return Key::P;

			case 0x1E:	return Key::A;
			case 0x1F:	return Key::S;
			case 0x20:	return Key::D;
			case 0x21:	return Key::F;
			case 0x22:	return Key::G;
			case 0x23:	return Key::H;
			case 0x24:	return Key::J;
			case 0x25:	return Key::K;
			case 0x26:	return Key::L;

			case 0x2C:	return Key::Z;
			case 0x2D:	return Key::X;
			case 0x2E:	return Key::C;
			case 0x2F:	return Key::V;
			case 0x30:	return Key::B;
			case 0x31:	return Key::N;
			case 0x32:	return Key::M;

			case 0x02:	return Key::Num1;
			case 0x03:	return Key::Num2;
			case 0x04:	return Key::Num3;
			case 0x05:	return Key::Num4;
			case 0x06:	return Key::Num5;
			case 0x07:	return Key::Num6;
			case 0x08:	return Key::Num7;
			case 0x09:	return Key::Num8;
			case 0x0A:	return Key::Num9;
			case 0x0B:	return Key::Num0;

			case 0x29:	return Key::GraveAccent;  // ` ~ (Left of 1)
			case 0x0C:	return Key::Minus;        // - _ (Right of 0)
			case 0x0D:	return Key::Equal;        // = + (Right of Minus)

			case 0x1A:	return Key::LeftBracket;  // [ { (Right of P)
			case 0x1B:	return Key::RightBracket; // ] } (Right of Left Bracket)
			case 0x2B:	return Key::Backslash;    // \ | (Right of Right Bracket / Above Enter)

			case 0x27:	return Key::Semicolon;    // ; : (Right of L)
			case 0x28:	return Key::Apostrophe;   // ' " (Right of Semicolon)

			case 0x33:	return Key::Comma;        // , < (Right of M)
			case 0x34:	return Key::Period;       // . > (Right of Comma)
			case 0x35:	return Key::Slash;        // / ? (Right of Period)

			default:	return Key::Unknown;
		}
	}

	Key GetKeyFromWinKeyCode(WPARAM wParam, LPARAM lParam)
	{
		// Alphabetic, numeric and symbol keys are mapped physically, to avoid issues with different keyboard layouts.
		// Functional keys are mapped logically, cause they are not affected by keyboard layouts.

		// Function can still return Key::Unknown at the end, this is just check if the key is mapped by scancode
		Key physicalKey = GetKeyFromScancode(lParam);
		if (physicalKey != Key::Unknown)
			return physicalKey;

		bool isExtended = (HIWORD(lParam) & KF_EXTENDED) != 0;

		switch (wParam)
		{
			case VK_SPACE:		return Key::Space;

			// VK_SHIFT needs special handling, using scancode to distinguish between left and right shift keys
			case VK_SHIFT:
			{
				UINT scanCode = (lParam & 0x00FF0000) >> 16;
				return MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT
					? Key::RightShift : Key::LeftShift;
			}

			case VK_CONTROL:	return isExtended ? Key::RightControl : Key::LeftControl;
			case VK_MENU:		return isExtended ? Key::RightAlt : Key::LeftAlt;

			case VK_LWIN:		return Key::LeftSuper;
			case VK_RWIN:		return Key::RightSuper;

			case VK_F1:			return Key::F1;
			case VK_F2:			return Key::F2;
			case VK_F3:			return Key::F3;
			case VK_F4:			return Key::F4;
			case VK_F5:			return Key::F5;
			case VK_F6:			return Key::F6;
			case VK_F7:			return Key::F7;
			case VK_F8:			return Key::F8;
			case VK_F9:			return Key::F9;
			case VK_F10:		return Key::F10;
			case VK_F11:		return Key::F11;
			case VK_F12:		return Key::F12;

			case VK_ESCAPE:		return Key::Escape;
			case VK_RETURN:		return isExtended ? Key::NumpadEnter : Key::Enter;
			case VK_TAB:		return Key::Tab;
			case VK_BACK:		return Key::Backspace;
			case VK_INSERT:		return Key::Insert;
			case VK_DELETE:		return Key::Delete;
			case VK_RIGHT:		return Key::Right;
			case VK_LEFT:		return Key::Left;
			case VK_DOWN:		return Key::Down;
			case VK_UP:			return Key::Up;
			case VK_PRIOR:		return Key::PageUp;
			case VK_NEXT:		return Key::PageDown;
			case VK_HOME:		return Key::Home;
			case VK_END:		return Key::End;
			case VK_CAPITAL:	return Key::CapsLock;
			case VK_SCROLL:		return Key::ScrollLock;
			case VK_NUMLOCK:	return Key::NumLock;
			case VK_SNAPSHOT:	return Key::PrintScreen;
			case VK_PAUSE:		return Key::Pause;

			case VK_NUMPAD0:	return Key::Numpad0;
			case VK_NUMPAD1:	return Key::Numpad1;
			case VK_NUMPAD2:	return Key::Numpad2;
			case VK_NUMPAD3:	return Key::Numpad3;
			case VK_NUMPAD4:	return Key::Numpad4;
			case VK_NUMPAD5:	return Key::Numpad5;
			case VK_NUMPAD6:	return Key::Numpad6;
			case VK_NUMPAD7:	return Key::Numpad7;
			case VK_NUMPAD8:	return Key::Numpad8;
			case VK_NUMPAD9:	return Key::Numpad9;

			case VK_DECIMAL:	return Key::NumpadDecimal;
			case VK_DIVIDE:		return Key::NumpadDivide;
			case VK_MULTIPLY:	return Key::NumpadMultiply;
			case VK_SUBTRACT:	return Key::NumpadSubtract;
			case VK_ADD:		return Key::NumpadAdd;
			case 0x92:			return Key::NumpadEqual;

			default:			return Key::Unknown;
		}
	}

	MouseButton GetButtonFromMessage(UINT msg, WPARAM wParam)
	{
		switch (msg)
		{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:	return MouseButton::Left;

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:	return MouseButton::Right;

			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:	return MouseButton::Middle;

			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:	return (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
									? MouseButton::Button4 : MouseButton::Button5;

			default:			return MouseButton::Unknown;
		}
	}
}
