#include "PrismSurface/Events.h"

namespace PrismSurface
{
	EventType WindowClosedEvent::GetType() const
	{
		return EventType::WindowClosed;
	}

	const char* WindowClosedEvent::GetName() const
	{
		return "WindowClosed";
	}

	EventType WindowResizedEvent::GetType() const
	{
		return EventType::WindowResized;
	}

	const char* WindowResizedEvent::GetName() const
	{
		return "WindowResized";
	}

	EventType WindowFocusedEvent::GetType() const
	{
		return EventType::WindowFocused;
	}

	const char* WindowFocusedEvent::GetName() const
	{
		return "WindowFocused";
	}

	EventType WindowLostFocusEvent::GetType() const
	{
		return EventType::WindowLostFocus;
	}

	const char* WindowLostFocusEvent::GetName() const
	{
		return "WindowLostFocus";
	}

	EventType WindowMovedEvent::GetType() const
	{
		return EventType::WindowMoved;
	}

	const char* WindowMovedEvent::GetName() const
	{
		return "WindowMoved";
	}

	EventType KeyPressedEvent::GetType() const
	{
		return EventType::KeyPressed;
	}

	const char* KeyPressedEvent::GetName() const
	{
		return "KeyPressed";
	}

	EventType KeyReleasedEvent::GetType() const
	{
		return EventType::KeyReleased;
	}

	const char* KeyReleasedEvent::GetName() const
	{
		return "KeyReleased";
	}

	EventType KeyTypedEvent::GetType() const
	{
		return EventType::KeyTyped;
	}

	const char* KeyTypedEvent::GetName() const
	{
		return "KeyTyped";
	}

	EventType MouseButtonPressedEvent::GetType() const
	{
		return EventType::MouseButtonPressed;
	}

	const char* MouseButtonPressedEvent::GetName() const
	{
		return "MouseButtonPressed";
	}

	EventType MouseButtonReleasedEvent::GetType() const
	{
		return EventType::MouseButtonReleased;
	}

	const char* MouseButtonReleasedEvent::GetName() const
	{
		return "MouseButtonReleased";
	}

	EventType MouseMovedEvent::GetType() const
	{
		return EventType::MouseMoved;
	}

	const char* MouseMovedEvent::GetName() const
	{
		return "MouseMoved";
	}

	EventType MouseScrolledEvent::GetType() const
	{
		return EventType::MouseScrolled;
	}

	const char* MouseScrolledEvent::GetName() const
	{
		return "MouseScrolled";
	}
}
