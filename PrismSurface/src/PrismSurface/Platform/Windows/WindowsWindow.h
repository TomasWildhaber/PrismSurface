#pragma once

#include <windows.h>

#include "PrismSurface/PrismSurfaceConfig.h"
#include "PrismSurface/Window.h"

namespace PrismSurface
{
	PRISM_SOURCE_API class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow();

		virtual const void* GetNativeHandle() const;

		void Update() override;

		virtual void SetTheme(Theme theme) override;
		void Resize(uint32_t width, uint32_t height) override;
		void SetFullscreen() override;
		void Minimize() override;
		void Maximize() override;
		void Restore() override;
		void Show() override;
		void Hide() override;
		void Close() override;

		HWND GetHandle() const { return m_WindowHandle; }
		HINSTANCE GetInstanceHandle() const { return m_InstanceHandle; }
	private:
		friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_WindowHandle;
		HINSTANCE m_InstanceHandle;
	};
}
