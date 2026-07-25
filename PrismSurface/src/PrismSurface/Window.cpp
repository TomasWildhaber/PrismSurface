#include "pch.h"

#include "PrismSurface/Window.h"

namespace PrismSurface
{
	uint32_t Window::GetWidth() const
	{
		return m_Properties.Width;
	}

	uint32_t Window::GetHeight() const
	{
		return m_Properties.Height;
	}

	std::string Window::GetTitle() const
	{
		return m_Properties.Title;
	}

	bool Window::IsResizable() const
	{
		return m_Properties.Resizable;
	}

	bool Window::HasCustomTitleBar() const
	{
		return m_Properties.CustomTitleBar;
	}

	bool Window::IsVisible() const
	{
		return m_Properties.Visible;
	}

	bool Window::IsMaximized() const
	{
		return m_Properties.CurrentState == WindowState::Maximized;
	}

	bool Window::IsMinimized() const
	{
		return m_Properties.CurrentState == WindowState::Minimized;
	}

	bool Window::IsFullscreen() const
	{
		return m_Properties.CurrentState == WindowState::Fullscreen;
	}
}
