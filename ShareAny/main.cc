#include <thread>
#include <vector>
#include <QtCore/qstring.h>
#include <iostream>
#include "web.h"
#include "gui.h"
/*
作者:CoioreLen
这个只负责启动剩下的程式
*/

void Run_QtGui(int argc,char **argv, std::vector<std::pair<QString, QString>> *dataList)
{
	QApplication app(argc, argv);
	ShareAnyWindow screen( (QWidget*)0,dataList);
	app.exec();
}

void Run_Web(std::vector<std::pair<QString, QString>>* dataList) {
	const char* a[] = { "", "--docroot",".","--http-address","0.0.0.0","--http-port","80","--resources-dir=./resources"};
	Wt::WRun(7, (char**)a, [dataList](const Wt::WEnvironment& env) {
		return std::make_unique<ShareAnyApplication>(env, dataList);
		});
}

int main(int argc,char **argv) {
	std::vector<std::pair<QString, QString>> dataList;
	std::thread t1(Run_QtGui,argc,argv,&dataList);
	std::thread t2(Run_Web,&dataList);
	t1.join();
	t2.join();

}