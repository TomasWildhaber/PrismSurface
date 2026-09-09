#pragma once

#include <string>
#include <cstdint>

#include "PrismSurface/PrismSurface.h"
#include "Events.h"

namespace PrismSurface
{
	enum class Theme
	{
		Light,
		Dark,
		System,
	};

	enum class WindowState
	{
		Minimized,
		Normal,
		Maximized,
		Fullscreen,
	};

	using EventCallbackFn = void(*)(Event&);

	struct WindowPosition
	{
		constexpr WindowPosition(int x, int y) : X(x), Y(y) {}

		// Since negative value are valid positions,
		// we use INT_MIN to represent centered position and INT_MAX to represent any position.

		constexpr static WindowPosition Centered()
		{
			return { INT_MIN, INT_MIN };
		}

		constexpr static WindowPosition AnyPosition()
		{
			return { INT_MAX, INT_MAX };
		}

		constexpr bool operator==(const WindowPosition& other) const
		{
			return X == other.X && Y == other.Y;
		}

		int X;
		int Y;
	};

	struct PRISM_API WindowProperties
	{
		uint32_t Width = 1280;
		uint32_t Height = 720;
		std::string Title;
		WindowPosition Position = WindowPosition::AnyPosition();
		WindowState CurrentState = WindowState::Normal;
		bool Visible = true;
		bool Resizable = true;
		bool DefaultTitleBar = true;
		bool Frame = true;
		bool WinMenu = false;
		bool AltF4Close = true;
		Theme CurrentTheme = Theme::System;
		EventCallbackFn EventCallback = nullptr;
	};

	class PRISM_API Window
	{
	public:
		virtual ~Window() = default;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		WindowPosition GetPosition() const;
		std::string GetTitle() const;
		Theme GetTheme() const;

		bool IsResizable() const;
		bool HasFrame() const;
		bool HasDefaultTitleBar() const;

		bool IsVisible() const;
		bool (IsMaximized)() const;
		bool (IsMinimized)() const;
		bool IsFullscreen() const;

		virtual const void* GetNativeHandle() const = 0;

		void SetEventCallback(EventCallbackFn callback);

		virtual void Update() = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual void SetTheme(Theme theme) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void SetFullscreen() = 0;

		virtual void Minimize() = 0;
		virtual void Maximize() = 0;
		virtual void Restore() = 0;
		virtual void Show() = 0;
		virtual void Hide() = 0;
		virtual void Close() = 0;

		static Window* Create(const WindowProperties& properties);
	protected:
		Window(const WindowProperties& properties) : m_Properties(properties) {}

		WindowProperties m_Properties;
	};
}
