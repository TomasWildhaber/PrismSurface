#include <windows.h>

#include "PrismSurfaceConfig.h"
#include "PrismSurface/Window.h"

namespace PrismSurface
{
	PRISM_SOURCE_API class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& properties);
		virtual ~WindowsWindow();

		virtual const void* GetNativeWindow() const;

		void Update() override;

		void Resize(uint32_t width, uint32_t height) override;
		void SetFullscreen() override;
		void Minimize() override;
		void Maximize() override;
		void Restore() override;
		void Show() override;
		void Hide() override;
		void Close() override;

		HWND GetWindowHandle() const { return m_WindowHandle; }
		HINSTANCE GetInstanceHandle() const { return m_InstanceHandle; }
	private:
		HWND m_WindowHandle;
		HINSTANCE m_InstanceHandle;
	};
}
