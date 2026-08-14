#include "PrismSurface/Window.h"

#include <iostream>

bool isRunning = true;

void OnEvent(PrismSurface::Event& event)
{
	if (event.GetType() == PrismSurface::EventType::WindowClosed)
	{
		isRunning = false;
	}

	std::cout << event.GetName() << std::endl;
}

int main(int argc, char** argv)
{
	PrismSurface::WindowProperties properties;
	properties.Title = "Example Window";
	properties.Width = 800;
	properties.Height = 600;
	properties.Centered = true;
	properties.EventCallback = OnEvent;

	PrismSurface::Window* window = PrismSurface::Window::Create(properties);

	while (isRunning)
	{
		window->Update();
	}

	delete window;

	return 0;
}