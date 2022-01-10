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
	std::vector<std::pair<QString, QString>> dataList;
    QApplication app(argc,argv);
    QStringList argvlist;
    for(int i=0;i<argc;i++){
        argvlist.append(QString(*(argv+i)));
        std::cout<<*(argv+i)<<std::endl;
    }
    ShareAnyWindow screen( (QWidget*)0,&dataList,argvlist);
    app.setQuitOnLastWindowClosed(true);
    app.exec();
}
