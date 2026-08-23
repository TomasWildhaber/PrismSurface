#pragma once

#include "PrismSurface/PrismSurface.h"

#include "KeyCodes.h"

namespace PrismSurface
{
	PRISM_API enum class EventType : uint8_t
	{
		WindowClosed,
		WindowFocused,
		WindowLostFocus,
		WindowResized,
		WindowMoved,
		TitlebarHittest,
		KeyPressed,
		KeyReleased,
		KeyTyped,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
	};

	PRISM_API class Event
	{
	public:
		virtual ~Event() = default;

		virtual EventType GetType() const = 0;
		virtual const char* GetName() const = 0;
	};

	PRISM_API class WindowClosedEvent : public Event
	{
	public:
		WindowClosedEvent() = default;

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	};

	PRISM_API class WindowFocusedEvent : public Event
	{
	public:
		WindowFocusedEvent() = default;

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	};

	PRISM_API class WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() = default;

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	};

	PRISM_API class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {
		}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		uint32_t m_Width, m_Height;
	};

	PRISM_API class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(uint32_t x, uint32_t y)
			: m_X(x), m_Y(y) {}

		uint32_t GetX() const { return m_X; }
		uint32_t GetY() const { return m_Y; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		uint32_t m_X, m_Y;
	};

	PRISM_API class TitlebarHittestEvent : public Event
	{
	public:
		enum class HittestResult : uint8_t
		{
			None,
			Titlebar,
			MinimizeButton,
			MaximizeButton,
			CloseButton,
		};

		TitlebarHittestEvent(float mouseX, float mouseY, HittestResult& hittest)
			: m_MouseX(mouseX), m_MouseY(mouseY), Hittest(hittest) {}

		float GetMouseX() const { return m_MouseX; }
		float GetMouseY() const { return m_MouseY; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;

		HittestResult& Hittest;
	private:
		float m_MouseX, m_MouseY;
	};

	PRISM_API class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(Key key)
			: m_Key(key) {}

		Key GetKey() const { return m_Key; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		Key m_Key;
	};

	PRISM_API class KeyReleasedEvent : public Event
	{
	public:
		KeyReleasedEvent(Key key)
			: m_Key(key) {}

		Key GetKey() const { return m_Key; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		Key m_Key;
	};

	PRISM_API class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(Key key)
			: m_Key(key) {}

		Key GetKey() const { return m_Key; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		Key m_Key;
	};

	PRISM_API class MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(MouseButton button)
			: m_Button(button) {}

		MouseButton GetMouseButton() const { return m_Button; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		MouseButton m_Button;
	};

	PRISM_API class MouseButtonReleasedEvent : public Event
	{
	public:
		MouseButtonReleasedEvent(MouseButton button)
			: m_Button(button) {}

		MouseButton GetMouseButton() const { return m_Button; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		MouseButton m_Button;
	};

	PRISM_API class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_X(x), m_Y(y) {}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		float m_X, m_Y;
	};

	PRISM_API class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float scroll, bool horizontal)
			: m_Scroll(scroll), m_Horizontal(horizontal) {}

		bool IsHorizontal() const { return m_Horizontal; }
		float GetScroll() const { return m_Scroll; }

		virtual EventType GetType() const override;
		virtual const char* GetName() const override;
	private:
		bool m_Horizontal;
		float m_Scroll;
	};
}
