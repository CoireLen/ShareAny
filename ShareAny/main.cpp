#include <thread>
#include <vector>
#include <QtCore/qstring.h>
#include <iostream>
#include "gui.h"
/*
����:CoioreLen
���ֻ��������ʣ�µĳ�ʽ
*/



int main(int argc,char **argv) {
	std::vector<std::pair<QString, QByteArray>> dataList;
    QApplication app(argc,argv);
    ShareAnyWindow screen( (QWidget*)0,&dataList);
    app.setQuitOnLastWindowClosed(true);
    app.exec();
}
