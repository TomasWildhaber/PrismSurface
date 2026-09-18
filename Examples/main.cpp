#include "PrismSurface/Window.h"
#include "PrismSurface/Error.h"

#include <iostream>

bool isRunning = true;
uint32_t windowWidth = 800;
PrismSurface::Window* window;

void OnEvent(PrismSurface::Event& event)
{
	switch (event.GetType())
	{
		case PrismSurface::EventType::WindowClosed:
		{
			isRunning = false;
			break;
		}

		case PrismSurface::EventType::WindowResized:
		{
			PrismSurface::WindowResizedEvent& resizedEvent = static_cast<PrismSurface::WindowResizedEvent&>(event);
			windowWidth = resizedEvent.GetWidth();
			break;
		}

		case PrismSurface::EventType::TitlebarHittest:
		{
			PrismSurface::TitlebarHittestEvent& hittestEvent = static_cast<PrismSurface::TitlebarHittestEvent&>(event);

			float titlebarHeight = 30.0f;
			if (hittestEvent.GetMouseY() < titlebarHeight)
			{
				if (hittestEvent.GetMouseX() >= windowWidth - 30.0f)
					hittestEvent.Hittest = PrismSurface::TitlebarHittestEvent::HittestResult::CloseButton;
				else if (hittestEvent.GetMouseX() >= windowWidth - 60.0f)
					hittestEvent.Hittest = PrismSurface::TitlebarHittestEvent::HittestResult::MaximizeButton;
				else if (hittestEvent.GetMouseX() >= windowWidth - 90.0f)
					hittestEvent.Hittest = PrismSurface::TitlebarHittestEvent::HittestResult::MinimizeButton;
				else
					hittestEvent.Hittest = PrismSurface::TitlebarHittestEvent::HittestResult::Titlebar;
			}

			break;
		}

		case PrismSurface::EventType::DragFileDropped:
		{
			PrismSurface::DragFileDroppedEvent& fileDroppedEvent = static_cast<PrismSurface::DragFileDroppedEvent&>(event);

			for (size_t i = 0; i < fileDroppedEvent.GetFileCount(); i++)
			{
				std::cout << "Dropped file: " << fileDroppedEvent.GetFiles()[i] << std::endl;
			}

			break;
		}
	}

	std::cout << event.GetName() << std::endl;
}

int main(int argc, char** argv)
{
	PrismSurface::ErrorHandler::SetErrorCallback([](PrismSurface::ErrorCode, const char* message)
	{
		std::cout << message << std::endl;
	});

	PrismSurface::WindowProperties properties;
	properties.Title = "Example Window";
	properties.Size = { 800, 600 };
	properties.Position = PrismSurface::WindowPosition::AnyPosition();
	properties.Resizable = true;
	properties.DefaultTitleBar = false;
	properties.Frame = true;
	properties.CurrentTheme = PrismSurface::Theme::Dark;
	properties.EventCallback = OnEvent;

	window = PrismSurface::Window::Create(properties);
	if (!window)
		return 1;

	while (isRunning)
	{
		window->Update();
	}

	delete window;

	return 0;
}
