#include "App.h"

#include <conio.h> //for _getch()

int main()
{
	App* app = new App();

	app->run(WORDCOUNT, TEST);

	_getch();
	return 0;
}