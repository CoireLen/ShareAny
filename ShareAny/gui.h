#include <QtWidgets/qlistwidget.h>
#include <QtGui/qbitmap.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/qtoolbar.h>
#include <QtCore/qtextcodec.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qfiledialog.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qmimedata.h>
#include <QtGui/QDropEvent>
#include <QtGui/qclipboard.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlistwidget.h>
#include <QtNetwork/qhostinfo.h>
#include <QtNetwork/QNetworkInterface>
#include <QtGui/qpainter.h>
#include <qrencode.h>
class ShareAnyListWidget :public QListWidget {
public:
	ShareAnyListWidget(std::vector<std::pair<QString, QString>> *dataList);
	~ShareAnyListWidget();
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void deleteAll();
	std::vector<std::pair<QString, QString>>* dataList = NULL;
private:
	void addList(QString,QString);
	
};

class ShareAnyWindow :public QWidget
{
public:
	ShareAnyWindow(QWidget* parent ,std::vector<std::pair<QString, QString>> *dataList);
	~ShareAnyWindow();
	void OnRemove();
	void OnShowQRcode();
	std::vector<std::pair<QString, QString>>* dataList = NULL;
private:
	QGridLayout layout;
	QToolBar toolBar;
	ShareAnyListWidget *SA_listwidget;
	
};

class FindStringLsit:public QStringList
{
public:
	bool Find(QString);
private:

};

class ShareAnyListWidgetItem :public QListWidgetItem
{
public:
	void setData(QString, QString);
	QString type;
	QString data;
	QWidget widget;
	QHBoxLayout layout;
	QLabel label; 
private:

};

class QRcodeWindow :public QWidget
{
public:
	QRcodeWindow(QWidget* parent, QString str);
	//~QRcodeWindow();

private:
	void GenerateQRcode(QString);
	QPixmap qrcodeimg;
	QLabel qrlabel;
	QGridLayout layout;
};
