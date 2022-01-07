#include "web.h"
#include "gui.h"
#include <iostream>
/*
�ⲿ��ʹ�� QT������gui
- ֧����ק�����ļ� ͼƬ ��Ƶ ����
- ֧�������Ϣ
- ֧���Ҽ������ı���Ϣ
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
	QString link;
	if (SA_Setting->isUseHttps()) {
		link = "https://"+ipAddress;
		if (SA_Setting->GetEndpoint() != "443") {
			link += ":" + SA_Setting->GetEndpoint();
		}
	}
	else {
		link = "http://" + ipAddress;
		if (SA_Setting->GetEndpoint() != "80") {
			link += ":" + SA_Setting->GetEndpoint();
		}
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
		addItemToList(mimedata->text());

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
	QStringList filepaths;
	if (paths.startsWith("file:///"))
	{
		filepaths = paths.split('\n');
	}
	else {
		filepaths.push_back(paths);
	}
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
	QRcode* qrcode; //��ά������
	//QR_ECLEVEL_Q �ݴ��ȼ�
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
void strcpyns(char* des, std::string src) {
	for (int i = 0; i < src.size(); i++) {
		*(des + i) = src[i];
	}
}

WtThread::WtThread(std::vector<std::pair<QString, QString>>* a, QString b ,std::string c, QString d,bool e,bool f)
{
	this->setData(a, b, c,d,e,f);
}
void WtThread::setData(std::vector<std::pair<QString, QString>>* a, QString b, std::string c, QString d,bool e,bool f) {
	datalist = a;
	endpoint = b;
	path = c;
	upFolder = d;
	useupload = e;
	usehttps = f;
}
void WtThread::serverstop() {
	server->stop();
}
void WtThread::run()
{
	auto dataList_ = this->datalist;
	auto upfolder_ = upFolder;
	auto useupload_ = useupload;
	static std::vector<std::string> startData;
	if (usehttps)
	{
		startData = { "", "--docroot",".","--https-address","0.0.0.0","--https-port","80",
		"--resources-dir=./resources","-c","./wt_config.xml" ,
		"--ssl-certificate=./server.pem","--ssl-private-key=./server.key","--ssl-tmp-dh=./dh2048.pem" , 
		"ssl.cipher-list=ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHER-RSA -AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES11 -SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA :DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256 -SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS- DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA" };
	}
	else {
		startData = { "", "--docroot",".","--http-address","0.0.0.0","--http-port","80","--resources-dir=./resources","-c","./wt_config.xml" };
	}
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
	server=new Wt::WServer(startData.size(), a, WTHTTP_CONFIGURATION);
	server->addEntryPoint(Wt::EntryPointType::Application,
		[dataList_,upfolder_, useupload_](const Wt::WEnvironment& env) {
			return std::make_unique<ShareAnyApplication>(env, dataList_, upfolder_, useupload_); },
		path,
				std::string(""));
	server->run();
	free(a);
	free(port);
}

QString getRandomString(int length)
{
	//qsrand(QDateTime::currentMSecsSinceEpoch());//Ϊ���ֵ�趨һ��seed

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
	//HTTPS启用选项
	usehttpscheck.setText("Use HTTPS");
	layout.addWidget(&usehttpscheck, 0, 0, 1, 2);
	//端口
	endpointlabel.setText("Prot:");
	//endpointlabel.setAlignment(Qt::AlignRight);
	layout.addWidget(&endpointlabel, 1, 0, 1, 1);
	endpointedit.setText("80");
	layout.addWidget(&endpointedit, 1, 1, 1, 1);
	//启用随机链接
	entrycheck.setText("Use RandLink");
	layout.addWidget(&entrycheck,2,0,1,2);
	//启用上传功能
	layout.addWidget(&uploadcheck, 3, 0, 1, 2);
	uploadcheck.setText("Use Upload");
	layout.addWidget(&uploadpath, 4, 0, 1, 1);
	uploadpath.setText("./upload/");
	//选择上传文件夹
	connect(&uploadbutton, &QPushButton::clicked, this, &ShareAnySettingWindow::OnSelectFolder);
	layout.addWidget(&uploadbutton, 4, 1, 1, 1);
	uploadbutton.setText("Select Folder");
	//应用
	apply.setText("Apply");
	connect(&apply, &QPushButton::clicked, this, &ShareAnySettingWindow::OnApply);
	layout.addWidget(&apply);
	//����Ŷ�ȡsetting.json�Ĵ���
	QFile json("./setting.json");
	json.open(QIODevice::ReadWrite);
	auto jsfiledata = json.readAll();
	std::cout << "JsonFileRead:" << jsfiledata.toStdString() << std::endl;
	QJsonParseError error;
	settingjson=QJsonDocument::fromJson(QString(jsfiledata).toUtf8(), &error);
	json.close();
	QString uploadfolder = "./upload/";
	bool useupload = false;
	bool usehttps = false;
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
			auto v3 = settingjson.object().take("useupload");
			if (v3.isBool()) {
				if (v3.toBool() == true) {
					uploadcheck.setCheckState(Qt::CheckState::Checked);
					useupload = true;
				}
			}
			auto v4 = settingjson.object().take("uploadpath");
			if (v4.isString()) {
				uploadpath.setText(v4.toString());
				uploadfolder = v4.toString();
			}
			auto v5 = settingjson.object().take("usehttps");
			if (v5.isBool()) {
				if (v5.toBool() == true) {
					usehttpscheck.setCheckState(Qt::CheckState::Checked);
					usehttps = true;
				}
			}
		}
	}
	dataList = data;
	webthread = new WtThread(data, endpointedit.text(),entrypath.toStdString(),uploadfolder, useupload,usehttps);
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
	webthread = new WtThread(this->dataList, endpointedit.text(), entrypath.toStdString(),
		uploadpath.text(),uploadcheck.checkState()== Qt::CheckState::Checked, usehttpscheck.checkState() == Qt::CheckState::Checked);
	webthread->start();
	//������д��setting.json
	QJsonObject obj;
	//��������
	obj.insert("endpoint", endpointedit.text());//�˿�
	obj.insert("entrypath", entrycheck.checkState()== Qt::CheckState::Checked);//�����������
	obj.insert("useupload", uploadcheck.checkState() == Qt::CheckState::Checked);
	obj.insert("uploadpath", uploadpath.text());
	obj.insert("usehttps", usehttpscheck.checkState() == Qt::CheckState::Checked);
	//
	auto jsdoc = QJsonDocument(obj);
	QFile json("./setting.json");
	json.open(QIODevice::WriteOnly);
	auto array = jsdoc.toJson();
	std::cout << "JsonArray:" << array.toStdString() << std::endl;
	json.write(array);
	json.close();
}
void ShareAnySettingWindow::OnSelectFolder() {
	this->uploadpath.setText(
	QFileDialog::getExistingDirectory(
		this, "UpLoad Directory",
		"/")+'/');
}
QString ShareAnySettingWindow::GetEndpoint() {
	return this->endpointedit.text();
}
QString ShareAnySettingWindow::GetEntryPath() {
	return this->entrypath;
}
bool ShareAnySettingWindow::isUseHttps() {
	return usehttpscheck.checkState() == Qt::CheckState::Checked;
}
