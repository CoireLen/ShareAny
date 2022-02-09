#include "web.h"
#include <QtWidgets/qlistwidget.h>
#include <QtGui/qbitmap.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/qtoolbar.h>
#include <QtCore/qtextcodec.h>
#include <QtCore/qthread.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcheckbox.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qmimedata.h>
#include <QtCore/qrandom.h>
#include <QtGui/QDropEvent>
#include <QtGui/qclipboard.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlistwidget.h>
#include <QtNetwork/qhostinfo.h>
#include <QtNetwork/QNetworkInterface>
#include <QtGui/qpainter.h>
#include <QtCore/qtemporarydir.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/QDateTime>
#include <qrencode.h>

class WtThread :public QThread
{
public:
	WtThread(std::vector<std::pair<QString, QByteArray>>* dataList,
        QString endpoint, std::string path, QString upFolder,bool useupload,bool usehttps);
	void run();
	void serverstop();
private:
	std::vector<std::pair<QString, QByteArray>>* datalist;
	QString endpoint;
	std::string path;
	QString upFolder;
	bool useupload;
	bool usehttps;
    Wt::WServer* server;
};

class ShareAnyListWidget :public QListWidget {
public:
	ShareAnyListWidget(std::vector<std::pair<QString, QByteArray>> *dataList);
	~ShareAnyListWidget();
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void deleteAll();
	std::vector<std::pair<QString, QByteArray>>* dataList = NULL;
	
private:
	void addList(QString,QString);
	void addItemToList(QString);
	void addItemfromMimedata(const QMimeData *);
	QTemporaryDir tempdir;
	QList<QString> tempimagefiles;
};
class ShareAnySettingWindow :public QWidget {
public:
    ShareAnySettingWindow(QWidget*, std::vector<std::pair<QString, QByteArray>>* data);
	~ShareAnySettingWindow();
	void OnSelectFolder();
	void OnApply();
	QString GetEndpoint();
	QString GetEntryPath();
	bool isUseHttps();
private:
	QGridLayout layout;
	QLabel endpointlabel;
	QCheckBox usehttpscheck;//����Ƿ�����https
	QLineEdit endpointedit;
	QCheckBox entrycheck;
	QCheckBox uploadcheck;//�����ļ��ϴ�����
	QLabel    uploadpath;//�ϴ��ļ�·����ʾ
	QPushButton uploadbutton; //ѡ���ϴ��洢·����
	QJsonDocument settingjson;
	QPushButton apply;
	std::vector<std::pair<QString, QByteArray>>* dataList;
	WtThread * webthread;
	QString entrypath = "";
    QStringList argv;
};
class ShareAnyWindow :public QWidget
{
public:
    ShareAnyWindow(QWidget* parent ,std::vector<std::pair<QString, QByteArray>> *dataList);
	~ShareAnyWindow();
	void OnRemove();
	void OnShowQRcode();
	void OnShowSetting();
	std::vector<std::pair<QString, QByteArray>>* dataList = NULL;
private:
	QGridLayout layout;
	QToolBar toolBar;
	ShareAnyListWidget *SA_listwidget;
	ShareAnySettingWindow *SA_Setting;
};

class FindStringLsit
{
public:
    FindStringLsit(std::vector<std::string> list);
	bool Find(QString);
private:
    std::vector<std::string> list;
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
	QRcodeWindow(QWidget* parent, QStringList str);
	//~QRcodeWindow();

private:
	void GenerateQRcode(QString);
	void SetClipboard(QString);
	QPixmap qrcodeimg;
	QLabel qrlabel;
	std::vector<QPushButton*> v_qrstr;
	QGridLayout layout;
};
//
