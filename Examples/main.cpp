#include "PrismSurface/Window.h"

bool isRunning = true;

void OnWindowClose()
{
	isRunning = false;
}

int main(int argc, char** argv)
{
	PrismSurface::WindowProperties properties;
	properties.Title = "Example Window";
	properties.Width = 800;
	properties.Height = 600;
	properties.Centered = true;

	PrismSurface::Window* window = PrismSurface::Window::Create(properties);

	while (isRunning)
	{
		window->Update();
	}

	delete window;

	return 0;
}