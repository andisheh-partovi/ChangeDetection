#include "App.h"

#include <conio.h> //for _getch()

int main()
{
	App* app = new App();

	app->run(wordCount);

	_getch();
	return 0;
}