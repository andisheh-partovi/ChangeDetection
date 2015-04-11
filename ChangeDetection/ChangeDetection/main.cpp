#include "App.h"

#include <conio.h> //for _getch()

int main()
{
	App* app = new App();

	app->run(FUNCTIONWORDCOUNT, STATE_OF_THE_UNION, false);

	_getch();
	return 0;
}