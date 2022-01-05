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
	app.setQuitOnLastWindowClosed(true);
	app.exec();
}

int main(int argc,char **argv) {
	std::vector<std::pair<QString, QString>> dataList;
	std::thread t1(Run_QtGui,argc,argv,&dataList);
	//std::thread t2(Run_Web,argc, argv,&dataList);
	t1.join();
	//t2.join();

}