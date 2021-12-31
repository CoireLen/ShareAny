#include "gui.h"
#include <iostream>
/*
�ⲿ��ʹ�� QT������gui
- ֧����ק�����ļ� ͼƬ ��Ƶ ����
- ֧�������Ϣ
- ֧���Ҽ�����ı���Ϣ
- urlɨ�����
*/
ShareAnyWindow::ShareAnyWindow(QWidget* parent,std::vector<std::pair<QString, QString>>* data) :QWidget(parent)
{
	this->dataList = data;
	this->setGeometry(0, 0, 450, 600);
	this->setLayout(&layout);
	this->setWindowTitle(QString("ShareAnyThing"));
	static QAction toolButton1(QString("Remove"));
	toolBar.addAction(&toolButton1);
	connect(&toolButton1, &QAction::triggered, this, &ShareAnyWindow::OnRemove);
	static QAction toolButton2(QString("QRcode"));
	toolBar.addAction(&toolButton2);
	connect(&toolButton2, &QAction::triggered, this, &ShareAnyWindow::OnShowQRcode);
	layout.addWidget(&toolBar);
	this->SA_listwidget = new ShareAnyListWidget(dataList);
	layout.addWidget(SA_listwidget);
	this->show();
	
}
void ShareAnyWindow::OnRemove() {
	this->dataList->clear();
	SA_listwidget->deleteAll();

}
void ShareAnyWindow::OnShowQRcode() {
	QString ipAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			ipAddress = ipAddressesList.at(i).toString();
			break;
		}
	}
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	std::cout << ipAddress.toStdString() << std::endl;
	new QRcodeWindow(this, "http://"+ipAddress);
}
ShareAnyWindow::~ShareAnyWindow() {
	delete SA_listwidget;
}
/*
������FindStringList
*/
bool FindStringLsit::Find(QString str) {
	for (int i = 0; i < this->length();i++) {
		if (this->at(i)==str) {
			return true;
		}
	}
	return false;
}
/*
	������LsitWeidgetItem
*/

void ShareAnyListWidgetItem::setData(QString type,QString data) {
	this->type = type;
	this->data = data;
	layout.addWidget(&label);
	widget.setLayout(&layout);
	label.setText(data);
	this->setSizeHint(QSize(0, 50));
}
/*
	������LsitWeidget
*/
ShareAnyListWidget::ShareAnyListWidget(std::vector<std::pair<QString, QString>>*data) {
	this->dataList = data;
	this->setViewMode(QListWidget::ListMode);
	this->setAcceptDrops(true);
	this->setDragEnabled(true);
	this->setDragDropMode(QAbstractItemView::InternalMove);           //�����Ϸ�
	this->setSelectionMode(QAbstractItemView::ExtendedSelection); //����ѡ����
	this->setDefaultDropAction(Qt::MoveAction);
	
	//this->setFlow(QListView::LeftToRight);
}
void ShareAnyListWidget::deleteAll() {
	for (int i = this->count() - 1; i >= 0; i--) {
		delete this->item(i);
	}
	this->clear();
}
ShareAnyListWidget::~ShareAnyListWidget() {
	deleteAll();
}
void ShareAnyListWidget::mouseReleaseEvent(QMouseEvent* e) {
	if (e->button() == Qt::RightButton)
	{
		auto clip= QGuiApplication::clipboard();
		this->addList("text",clip->text());
	}
	else {
		e->accept();
	}
}
void ShareAnyListWidget::dragEnterEvent(QDragEnterEvent *e) {
	if (e->mimeData()->hasText()) {
		e->acceptProposedAction();
	}
	else {
		e->accept();
	}
}
void ShareAnyListWidget::dropEvent(QDropEvent *e) 
{
	if (e->mimeData()->hasText()) {
		if (e->mimeData()->text().startsWith("file:///")) {
			QStringList filepaths = e->mimeData()->text().split('\n');
			for (QString path : filepaths) {
				if (path.startsWith("file:///")) {
					path.replace("file:///","");
					std::cout << path.toStdString() << std::endl;
					QStringList a=path.split('.');
					if (FindStringLsit({ "jpg", "png", "gif" }).Find(a.at(a.length()-1))) {
						this->addList("image", path);
					}
					else if (FindStringLsit({ "mp3", "aac", "flac" }).Find(a.at(a.length() - 1))) {
						this->addList("audio", path);
					}
					else if (FindStringLsit({ "mp4"}).Find(a.at(a.length() - 1))) {
						this->addList("video", path);
					}
					else { 
						this->addList("file", path); 
					}
				}
			}
		}
	}
	else {
		e->accept();
	}
}
void ShareAnyListWidget::addList(QString type,QString data) {
	std::cout << type.toStdString() << ":" << data.toStdString() << std::endl;
	if (this->dataList!=NULL)
		this->dataList->push_back(std::pair<QString,QString>(type, data));
	std::vector<std::pair<QString, QString>>vp1;
	vp1.push_back(std::pair(type, data));
	auto item =new ShareAnyListWidgetItem();
	item->setData(type, data);
	this->addItem(item);
	this->setItemWidget(item,&item->widget);
}
QRcodeWindow::QRcodeWindow(QWidget* parent,QString str) {
	this->setWindowTitle("QRcode Website");
	this->resize(QSize(300, 300));
	this->GenerateQRcode(str);
	this->setLayout(&this->layout);
	this->layout.addWidget(&this->qrlabel);
	this->qrlabel.setPixmap(this->qrcodeimg);
	this->show();
}
void QRcodeWindow::GenerateQRcode(QString tempstr)
{
	QRcode* qrcode; //��ά������
	//QR_ECLEVEL_Q �ݴ�ȼ�
	qrcode = QRcode_encodeString(tempstr.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);
	qint32 temp_width = this->width(); //��ά��ͼƬ�Ĵ�С
	qint32 temp_height = this->height();
	qint32 qrcode_width = qrcode->width > 0 ? qrcode->width : 1;
	double scale_x = (double)temp_width / (double)qrcode_width; //��ά��ͼƬ�����ű���
	double scale_y = (double)temp_height / (double)qrcode_width;
	QImage mainimg = QImage(temp_width, temp_height, QImage::Format_ARGB32);
	QPainter painter(&mainimg);
	QColor background(Qt::white);
	painter.setBrush(background);
	painter.setPen(Qt::NoPen);
	painter.drawRect(0, 0, temp_width, temp_height);
	QColor foreground(Qt::black);
	painter.setBrush(foreground);
	for (qint32 y = 0; y < qrcode_width; y++)
	{
		for (qint32 x = 0; x < qrcode_width; x++)
		{
			unsigned char b = qrcode->data[y * qrcode_width + x];
			if (b & 0x01)
			{
				QRectF r(x * scale_x, y * scale_y, scale_x, scale_y);
				painter.drawRects(&r, 1);
			}
		}
	}
	this->qrcodeimg = QPixmap::fromImage(mainimg);
}