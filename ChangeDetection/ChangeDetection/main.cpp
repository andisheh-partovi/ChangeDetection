#include "App.h"

#include <conio.h> //for _getch()

int main()
{
	App* app = new App();

	/*Give execution parameters:
	Method: what features to use
	algorithmNumber: 1 for our original idea and 2 for alpha.beta
	doParse: can be set to false if algorithm was ran once so save a bit of time
	Order: Method method, DataSet dataSet, bool doParse, bool isLogSpace, int algorithmNumber
	*/
	app->run(WORDCOUNT, STATE_OF_THE_UNION, true, true, 4);

	_getch();
	return 0;
}