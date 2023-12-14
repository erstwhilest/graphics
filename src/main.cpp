#include "application.h"

int main()
{
	Application* app{new Application()};
	callback=app;
	app->run();
	app->cleanUp();
	delete app;
}
