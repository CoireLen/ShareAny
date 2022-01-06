#include "web.h"
#include "gui.h"
#include <iostream>
/*
这部分使用 QT来创建gui
- 支持托拽进入文件 图片 音频 分享
- 支持清空信息
- 支持右键添加文本信息
- url扫码进入
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
	static QAction toolButton3(QString("Setting"));
	toolBar.addAction(&toolButton3);
	connect(&toolButton3, &QAction::triggered, this, &ShareAnyWindow::OnShowSetting);
	layout.addWidget(&toolBar);
	this->SA_listwidget = new ShareAnyListWidget(dataList);
	this->SA_Setting = new ShareAnySettingWindow(this,data);
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
	auto link = "http://" + ipAddress;
	if (SA_Setting->GetEndpoint() != "80") {
		link += ":" + SA_Setting->GetEndpoint();
	}
	if (SA_Setting->GetEntryPath() != "") {
		link +="/" + SA_Setting->GetEntryPath();
	}
	new QRcodeWindow(this, link);
}
void ShareAnyWindow::OnShowSetting() {
	SA_Setting->show();
}
ShareAnyWindow::~ShareAnyWindow() {
	delete SA_listwidget;
	delete SA_Setting;
}
/*
下面是FindStringList
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
	下面是LsitWeidgetItem
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
	下面是LsitWeidget
*/
ShareAnyListWidget::ShareAnyListWidget(std::vector<std::pair<QString, QString>>*data) {
	this->dataList = data;
	this->setViewMode(QListWidget::ListMode);
	this->setAcceptDrops(true);
	this->setDragEnabled(true);
	this->setDragDropMode(QAbstractItemView::InternalMove);           //设置拖放
	this->setSelectionMode(QAbstractItemView::ExtendedSelection); //设置选择多个
	this->setDefaultDropAction(Qt::MoveAction);
	
	//this->setFlow(QListView::LeftToRight);
}
void ShareAnyListWidget::deleteAll() {
	for (int i = this->count() - 1; i >= 0; i--) {
		delete this->item(i);
	}
	this->clear();
	for (auto i : tempimagefiles) {
			QFile::remove(i);
	}
}
ShareAnyListWidget::~ShareAnyListWidget() {
	deleteAll();
}
void ShareAnyListWidget::addItemfromMimedata(const QMimeData *mimedata) {
	if (mimedata->hasImage()) {
		QImage img = qvariant_cast<QImage>(mimedata->imageData());
		auto tempfilepath = tempdir.path() + "/" +QString::number(tempimagefiles.size())+".png";
		tempimagefiles.push_back(tempfilepath);
		std::cout << tempfilepath.toStdString() << std::endl;
		
		img.save(tempfilepath);
		addList("image", tempfilepath);
	}
	else if (mimedata->hasText()) {
		this->addItemToList(mimedata->text());
	}
}
void ShareAnyListWidget::mouseReleaseEvent(QMouseEvent* e) {
	if (e->button() == Qt::RightButton)
	{
		auto e = QGuiApplication::clipboard();
		addItemfromMimedata(e->mimeData());
	}
	else {
		e->accept();
	}
}
void ShareAnyListWidget::dragEnterEvent(QDragEnterEvent *e) {
	if (e->mimeData()->hasText()) {
		e->acceptProposedAction();
	}
	else if (e->mimeData()->hasImage()) {
		e->acceptProposedAction();
	}
	else {
		e->accept();
	}
}
void ShareAnyListWidget::addItemToList(QString paths) {
	QStringList filepaths = paths.split('\n');
	for (QString path : filepaths) {
		if (path.startsWith("file:///")) {
			path.replace("file:///", "");
			QStringList a = path.split('.');
			if (FindStringLsit({ "jpg", "png", "gif" }).Find(a.at(a.length() - 1))) {
				this->addList("image", path);
			}
			else if (FindStringLsit({ "mp3", "aac", "flac" }).Find(a.at(a.length() - 1))) {
				this->addList("audio", path);
			}
			else if (FindStringLsit({ "mp4" }).Find(a.at(a.length() - 1))) {
				this->addList("video", path);
			}
			else {
				this->addList("file", path);
			}
		}
		else {
			if (path!="")
				this->addList("text", path);
		}
	}
}
void ShareAnyListWidget::dropEvent(QDropEvent *e) 
{
	if (e->mimeData()->hasImage()) {
		addItemfromMimedata(e->mimeData());
	}
	else if (e->mimeData()->hasUrls()) {
		addItemfromMimedata(e->mimeData());
	}
	else if (e->mimeData()->hasText()) {
		addItemfromMimedata(e->mimeData());
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
	this->layout.addWidget(&this->qrstr);
	this->qrstr.setText(str);
	this->layout.addWidget(&this->qrlabel);
	this->qrlabel.setPixmap(this->qrcodeimg);
	connect(&this->qrstr, &QPushButton::clicked, this, &QRcodeWindow::SetClipboard);
	//connect(&toolButton3, &QAction::triggered, this, &ShareAnyWindow::OnShowSetting);
	this->show();
}
void QRcodeWindow::SetClipboard() {
	QGuiApplication::clipboard()->setText(qrstr.text());
	
}
void QRcodeWindow::GenerateQRcode(QString tempstr)
{
	QRcode* qrcode; //二维码数据
	//QR_ECLEVEL_Q 容错等级
	qrcode = QRcode_encodeString(tempstr.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);
	qint32 temp_width = this->width(); //二维码图片的大小
	qint32 temp_height = this->height();
	qint32 qrcode_width = qrcode->width > 0 ? qrcode->width : 1;
	double scale_x = (double)temp_width / (double)qrcode_width; //二维码图片的缩放比例
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
void strcpyns(char* des, std::string src) {
	for (int i = 0; i < src.size(); i++) {
		*(des + i) = src[i];
	}
}
//std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env, std::vector<std::pair<QString, QString>>* dataList)
//{
//	//auto app = std::make_unique<Wt::WApplication>(env);
//	auto app = std::make_unique<ShareAnyApplication>(env, dataList);
//	//app->root()->addWidget(std::make_unique<ShareAnyApplication>(env,dataList));
//
//	return app;
//}
WtThread::WtThread(std::vector<std::pair<QString, QString>>* a, QString b ,std::string c)
{
	this->setData(a, b, c);
}
void WtThread::setData(std::vector<std::pair<QString, QString>>* a, QString b, std::string c) {
	datalist = a;
	endpoint = b;
	path = c;
}
void WtThread::serverstop() {
	server->stop();
}
void WtThread::run()
{
	auto dataList = this->datalist;
	static std::vector<std::string> startData = { "", "--docroot",".","--http-address","0.0.0.0","--http-port","80","--resources-dir=./resources" };
	char** a = (char**)malloc(startData.size() * sizeof(char*));
	size_t size1 = endpoint.toStdString().size() * sizeof(char);
	char* port = (char*)calloc(endpoint.toStdString().size()+1,sizeof(char));
	std::cout << "Endpoint:" << endpoint.toStdString() << std::endl;
	std::cout << "EntryPath:" << path << std::endl;
	strcpyns(port, endpoint.toStdString());
	for (int i = 0; i < startData.size(); i++) {
		if (startData[i].starts_with("80")) {
			*(a + i) = port;
		}
		else {
			*(a + i) = (char*)startData[i].c_str();
		}
		puts(*(a + i));
	}
	server=new Wt::WServer(7, a, WTHTTP_CONFIGURATION);
	server->addEntryPoint(Wt::EntryPointType::Application,
		[dataList](const Wt::WEnvironment& env) {
			return std::make_unique<ShareAnyApplication>(env, dataList); },
		path,
				std::string(""));
	server->run();
	free(a);
	free(port);
}
/*//废弃的代码
void Run_Web(std::vector<std::pair<QString, QString>>* dataList, QString endpoint,const std::string &path) {
	static std::vector<std::string> startData = {"", "--docroot",".","--http-address","0.0.0.0","--http-port","80","--resources-dir=./resources"};
	char** a = (char **)malloc(startData.size()*sizeof(char*));
	size_t size1 = endpoint.toStdString().size() * sizeof(char);
	char * port= (char*)malloc( size1+1);
	std::cout << "Endpoint:" << endpoint.toStdString() << std::endl;
	std::cout << "EntryPath:" << path << std::endl;
	strcpyns(port,endpoint.toStdString());
	for (int i = 0; i < startData.size();i++) {
		if (startData[i].starts_with("80")) {
			*(a + i) = port;
		}
		else {
			*(a + i) = (char*)startData[i].c_str();
		}
		puts(*(a+i));
	}
	Wt::WServer server(7, a, WTHTTP_CONFIGURATION);
	server.addEntryPoint(Wt::EntryPointType::Application,
		[dataList](const Wt::WEnvironment& env) {
			return std::make_unique<ShareAnyApplication>(env, dataList); },
		path, 
		std::string(""));
	//server.addEntryPoint(EntryPointType::Application, createApplication);
	//Wt::WRun(7, a, [dataList](const Wt::WEnvironment& env) {
	//return std::make_unique<ShareAnyApplication>(env, dataList);
	//});
	server.run();
	free(a);
	free(port);
}*/
QString getRandomString(int length)
{
	//qsrand(QDateTime::currentMSecsSinceEpoch());//为随机值设定一个seed

	QRandomGenerator qrand(QDateTime::currentMSecsSinceEpoch());
	const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int chrs_size = sizeof(chrs);

	char* ch = new char[length + 1];
	memset(ch, 0, length + 1);
	int randomx = 0;
	for (int i = 0; i < length; ++i)
	{
		randomx = qrand.bounded(chrs_size-1);
		ch[i] = chrs[randomx];
	}

	QString ret(ch);
	delete[] ch;
	return ret;
}
ShareAnySettingWindow::ShareAnySettingWindow(QWidget* parent, std::vector<std::pair<QString, QString>>* data) {
	this->setWindowTitle("Setting");
	this->setLayout(&layout);
	endpointlabel.setText("Prot:");
	endpointlabel.setAlignment(Qt::AlignRight);
	layout.addWidget(&endpointlabel, 0, 0, 1, 1);
	endpointedit.setText("80");
	layout.addWidget(&endpointedit, 0, 1, 1, 1);
	apply.setText("Apply");
	entrycheck.setText("Use RandLink");
	layout.addWidget(&entrycheck,1,0,1,2);
	connect(&apply, &QPushButton::clicked, this, &ShareAnySettingWindow::OnApply);
	layout.addWidget(&apply);
	//这里放读取setting.json的代码
	QFile json("./setting.json");
	json.open(QIODevice::ReadWrite);
	auto jsfiledata = json.readAll();
	std::cout << "JsonFileRead:" << jsfiledata.toStdString() << std::endl;
	QJsonParseError error;
	settingjson=QJsonDocument::fromJson(QString(jsfiledata).toUtf8(), &error);
	json.close();
	if (error.error == QJsonParseError::NoError) {
		if (settingjson.isObject()) {
			auto v1 = settingjson.object().take("endpoint");
			if (v1.isString()) {
				endpointedit.setText(v1.toString());
			}
			auto v2 = settingjson.object().take("entrypath");
			if (v2.isBool()) {
				if (v2.toBool() == true) {
					this->entrycheck.setCheckState(Qt::CheckState::Checked);
					entrypath = getRandomString(8);
				}
			}
		}
	}
	dataList = data;
	webthread = new WtThread(data, endpointedit.text(),entrypath.toStdString());
	webthread->start();
}
ShareAnySettingWindow::~ShareAnySettingWindow() {
	webthread->serverstop();
	webthread->quit();
}
void ShareAnySettingWindow::OnApply() {
	webthread->serverstop();
	webthread->quit();
	if (entrycheck.checkState() == Qt::CheckState::Checked) {
		entrypath = getRandomString(8);
	}
	else {
		entrypath = "";
	}
	webthread = new WtThread(this->dataList, endpointedit.text(), entrypath.toStdString());
	webthread->start();
	//将数据写入setting.json
	QJsonObject obj;
	//添加数据
	obj.insert("endpoint", endpointedit.text());//端口
	obj.insert("entrypath", entrycheck.checkState()== Qt::CheckState::Checked);//启用随机链接
	//
	auto jsdoc = QJsonDocument(obj);
	QFile json("./setting.json");
	json.open(QIODevice::WriteOnly);
	auto array = jsdoc.toJson();
	std::cout << "JsonArray:" << array.toStdString() << std::endl;
	json.write(array);
	json.close();
}
QString ShareAnySettingWindow::GetEndpoint() {
	return this->endpointedit.text();
}
QString ShareAnySettingWindow::GetEntryPath() {
	return this->entrypath;
}

