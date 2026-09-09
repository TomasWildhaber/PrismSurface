#include "PrismSurface/Window.h"
#include "PrismSurface/Error.h"

#include <iostream>

bool isRunning = true;
uint32_t windowWidth = 800;

void OnEvent(PrismSurface::Event& event)
{
	if (event.GetType() == PrismSurface::EventType::WindowClosed)
	{
		isRunning = false;
	}

	if (event.GetType() == PrismSurface::EventType::KeyPressed)
	{
		PrismSurface::KeyPressedEvent& keyEvent = static_cast<PrismSurface::KeyPressedEvent&>(event);
		std::cout << "Key Pressed: " << (uint16_t)keyEvent.GetKey() << std::endl;
	}

	if (event.GetType() == PrismSurface::EventType::KeyReleased)
	{
		PrismSurface::KeyReleasedEvent& keyEvent = static_cast<PrismSurface::KeyReleasedEvent&>(event);
		std::cout << "Key Released: " << (uint16_t)keyEvent.GetKey() << std::endl;
	}
	
	if (event.GetType() == PrismSurface::EventType::WindowResized)
	{
		PrismSurface::WindowResizedEvent& resizedEvent = static_cast<PrismSurface::WindowResizedEvent&>(event);
		windowWidth = resizedEvent.GetWidth();
	}

	if (event.GetType() == PrismSurface::EventType::TitlebarHittest)
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
	}

	//std::cout << event.GetName() << std::endl;
}

int main(int argc, char** argv)
{
	PrismSurface::ErrorHandler::SetErrorCallback([](PrismSurface::ErrorCode, const char* message)
	{
		std::cout << message << std::endl;
	});

	PrismSurface::WindowProperties properties;
	properties.Title = "Example Window";
	properties.Width = 800;
	properties.Height = 600;
	properties.Position = PrismSurface::WindowPosition::AnyPosition();
	properties.Resizable = true;
	properties.DefaultTitleBar = false;
	properties.Frame = true;
	properties.CurrentTheme = PrismSurface::Theme::Dark;
	properties.EventCallback = OnEvent;

	PrismSurface::Window* window = PrismSurface::Window::Create(properties);
	if (!window)
		return 1;

	while (isRunning)
	{
		window->Update();
	}

	delete window;

	return 0;
}
