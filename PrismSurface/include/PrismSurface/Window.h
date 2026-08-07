#include <string>
#include <cstdint>

#include "PrismSurface.h"

namespace PrismSurface
{
	PRISM_API enum class WindowState
	{
		Minimized,
		Normal,
		Maximized,
		Fullscreen,
	};

	PRISM_API struct WindowProperties
	{
		uint32_t Width = 1280;
		uint32_t Height = 720;
		std::string Title;
		std::pair<uint32_t, uint32_t> Position = {0, 0};
		WindowState CurrentState = WindowState::Normal;
		bool Visible = true;
		bool Centered = false;
		bool Resizable = true;
		bool DefaultTitleBar = true;
		bool DefaultFrame = true;
	};

	PRISM_API class Window
	{
	public:
		virtual ~Window() = default;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		std::string GetTitle() const;

		bool IsResizable() const;
		bool HasDefaultTitleBar() const;

		bool IsVisible() const;
		bool IsMaximized() const;
		bool IsMinimized() const;
		bool IsFullscreen() const;

		virtual const void* GetNativeWindow() const = 0;

		virtual void Update() = 0;

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
