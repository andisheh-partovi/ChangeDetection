#include "App.h"

#include <boost\tokenizer.hpp>


App::App(void)
{
}

void App::run()
{
	std::string text = "token, test   string, M.B.A. Mr. Andrew hello.hi";

    boost::char_separator<char> sep(", ");
    boost::tokenizer< boost::char_separator<char> > tokens(text, sep);
    BOOST_FOREACH(const std::string& t, tokens) {
        std::cout << t << std::endl;
    }

	//std::string s = "This is,  a test M.B.A";
	//boost::tokenizer<> tok(s);
	//for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
	//{
	//	std::cout << *beg << "\n";
	//}
}

App::~App(void)
{
}
