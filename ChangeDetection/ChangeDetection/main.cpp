#include "App.h"

#include <conio.h> //for _getch()

int main()
{
	App* app = new App();

	//Method method, DataSet dataSet, bool doParse, bool isLogSpace, int algorithmNumber
	app->run(WORDCOUNT, TEST, false, true, 2);

	_getch();
	return 0;
}