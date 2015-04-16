#include "App.h"

#include <conio.h> //for _getch()

int main()
{
	App* app = new App();

	//Method method, DataSet dataSet, bool doParse, bool isLogSpace
	app->run(WORDCOUNT, TEST, false, true);

	_getch();
	return 0;
}