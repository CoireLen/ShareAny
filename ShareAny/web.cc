#include "web.h"

/*
建立web服务
展示页面
*/
ShareAnyWebApplication::ShareAnyWebApplication(const Wt::WEnvironment& env, std::vector<std::pair<QString, QByteArray>>* dataList,QString upFolder,bool useupload) : WApplication(env)
{
	if (upFolder != "") {
		this->uploadFolder = upFolder;
		std::cout << "UseFolder:" << upFolder.toStdString() << std::endl;
	}
	this->dataList = dataList;
    setTheme(std::make_shared<Wt::WBootstrapTheme>());
	setTitle("ShareAny");
	root()->setContentAlignment(Wt::AlignmentFlag::Center);
	//这上传区
	if (useupload) {
		drop_ = root()->addNew<Wt::WFileDropWidget>();

		drop_->setDropIndicationEnabled(true);
		// drop_->setGlobalDropEnabled(true);

        drop_->drop().connect(this, &ShareAnyWebApplication::handleDrop);
        drop_->newUpload().connect(this, &ShareAnyWebApplication::updateProgressListener);
        drop_->uploaded().connect(this, &ShareAnyWebApplication::saveFile);
        drop_->uploadFailed().connect(this, &ShareAnyWebApplication::failed);
        drop_->tooLarge().connect(this, &ShareAnyWebApplication::tooLarge);

		log_ = root()->addWidget(std::make_unique<Wt::WText>());
		log_->setInline(false);
		log_->setTextFormat(Wt::TextFormat::XHTML);

		Wt::WPushButton* abort = root()->addNew<Wt::WPushButton>("Abort current upload");
        abort->clicked().connect(this, &ShareAnyWebApplication::cancelUpload);
	}
	//以下主体动态代码
	
	for (auto i : *dataList) {
		std::cout << i.second.toStdString() << std::endl;
		if (i.first == "text") {
			
			auto container = root()->addNew<Wt::WContainerWidget>();
			container->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			auto hbox = container->setLayout(std::make_unique<Wt::WHBoxLayout>());
			QString text=QTextCodec::codecForLocale()->toUnicode(i.second);
			auto input1 = std::make_unique<Wt::WTextArea>(text.toUtf8().toStdString());
			input1->setMaximumSize(Wt::WLength("70%"), Wt::WLength(""));
			auto inputid = ("textarea" + QString::number(id++)).toStdString();
			input1->setId(inputid);
			hbox->addWidget(std::move(input1));
			
			static Wt::JSlot js= Wt::JSlot("document.getElementById('" + inputid + "').select();execCommand('Copy');alert('Copyed');");
			auto pushbutton = std::make_unique<Wt::WPushButton>("COPY");
			pushbutton->clicked().connect(js);
			pushbutton->setMaximumSize(Wt::WLength("30%"), Wt::WLength(""));
			hbox->addWidget(std::move(pushbutton));
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "image") {
            auto textResource = std::make_shared<Wt::WFileResource>(i.second.toStdString());
			Wt::WLink link = Wt::WLink(textResource);
			link.setTarget(Wt::LinkTarget::Self);
			auto panel = root()->addNew<Wt::WPanel>();
			panel->addStyleClass("centered-example");
			auto img=panel->setCentralWidget(std::make_unique<Wt::WImage>(link));
			//auto img=root()->addNew<Wt::WImage>(link);
			img->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			 QString text=QTextCodec::codecForLocale()->toUnicode(i.second);
            img->setAlternateText(text.toUtf8().toStdString());
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "audio") {
			auto container = root()->addNew<Wt::WContainerWidget>();
			container->addStyleClass("centered-example");
			container->setMinimumSize(Wt::WLength("100%"),Wt::WLength("80px"));
			auto hbox = container->setLayout(std::make_unique<Wt::WHBoxLayout>());
			 QString text=QTextCodec::codecForLocale()->toUnicode(i.second);
			auto filename = text.split('/');
            auto textResource = std::make_shared<Wt::WFileResource>(i.second.toStdString());
			textResource->suggestFileName(filename.at(filename.length() - 1).toUtf8().toStdString());
			Wt::WLink link = Wt::WLink(textResource);
			link.setTarget(Wt::LinkTarget::NewWindow);
			auto musicfile=std::make_unique<Wt::WAnchor>(link, filename.at(filename.length() - 1).toUtf8().toStdString());
			//musicfile->setMaximumSize(Wt::WLength("50%"), Wt::WLength("50%"));
			hbox->addWidget(std::move(musicfile));

			auto audio= std::make_unique<Wt::WAudio>();
			audio->addSource(Wt::WLink(link));
			audio->setOptions(Wt::PlayerOption::Controls);
			//audio->setMaximumSize(Wt::WLength("50%"), Wt::WLength("50%"));
			audio->setAlternativeContent
			(std::make_unique<Wt::WText>("You don't have HTML5 audio support!"));

			hbox->addWidget(std::move(audio));
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "file") {
            auto textResource = std::make_shared<Wt::WFileResource>("plain/text",i.second.toStdString());
			 QString text=QTextCodec::codecForLocale()->toUnicode(i.second);
			auto filename = text.split('/');
			textResource->suggestFileName(filename.at(filename.length() - 1).toUtf8().toStdString());
			Wt::WLink link = Wt::WLink(textResource);
			link.setTarget(Wt::LinkTarget::NewWindow);
			auto panel = root()->addNew<Wt::WPanel>();
			panel->addStyleClass("centered-example");
			panel->setCentralWidget(std::make_unique<Wt::WAnchor>(link, filename.at(filename.length() - 1).toUtf8().toStdString()));
			//root()->addNew<Wt::WAnchor>(link,filename.at(filename.length()-1).toStdString());
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "video") {
			auto container = root()->addNew<Wt::WContainerWidget>();
			container->addStyleClass("centered-example");
			container->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
            auto mp4Video = std::make_shared<Wt::WFileResource>(i.second.toStdString());
			 QString text=QTextCodec::codecForLocale()->toUnicode(i.second);
			auto filename = text.split('/');
			mp4Video->suggestFileName(filename.at(filename.length() - 1).toUtf8().toStdString());
			auto player = container->addNew<Wt::WVideo>();
			player->addSource(Wt::WLink(mp4Video));
			player->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			container->addNew<Wt::WBreak>();
			Wt::WLink link = Wt::WLink(mp4Video);
			link.setTarget(Wt::LinkTarget::NewWindow);
			container->addNew<Wt::WAnchor>(link, filename.at(filename.length() - 1).toUtf8().toStdString());
			root()->addNew<Wt::WBreak>();
		}
	}
	
}

void ShareAnyWebApplication::handleDrop(std::vector<Wt::WFileDropWidget::File*> files)
{
	for (unsigned i = 0; i < files.size(); i++) {
		Wt::WFileDropWidget::File* file = files[i];
		if (nbUploads_ >= maxfiles) {
			drop_->cancelUpload(file);
			continue;
		}

		auto block = drop_->addNew<Wt::WContainerWidget>();
		block->setToolTip(file->clientFileName() + " [" + file->mimeType() + "]");
		block->addStyleClass("upload-block");

		icons_[file] = block;
		nbUploads_++;
	}

	if (nbUploads_ >= maxfiles) {
		log_->setText("That's enough ...");
		drop_->setAcceptDrops(false);
	}
}

void ShareAnyWebApplication::cancelUpload()
{
	if (drop_->uploads().size() == drop_->currentIndex())
		return;

	Wt::WFileDropWidget::File* currentFile = drop_->uploads()[drop_->currentIndex()];
	drop_->cancelUpload(currentFile);
	icons_[currentFile]->addStyleClass("cancelled");
}

void ShareAnyWebApplication::tooLarge(Wt::WFileDropWidget::File* file, ::uint64_t)
{
	icons_[file]->addStyleClass("invalid");

	log_->setText("File too large: " + file->clientFileName());
}

void ShareAnyWebApplication::failed(Wt::WFileDropWidget::File* file)
{
	icons_[file]->addStyleClass("invalid");

	log_->setText("Upload failed: " + file->clientFileName());
}

void ShareAnyWebApplication::saveFile(Wt::WFileDropWidget::File* file)
{
	std::string spool = file->uploadedFile().spoolFileName();
	std::ifstream src(spool.c_str(), std::ios::binary);
	std::string filename {this->codec->fromUnicode(file->clientFileName().c_str()).toStdString()};
	std::string saveName = uploadFolder.toUtf8().toStdString() +filename ;
	
	std::cout <<"uoload file name:"<<filename<<std::endl;
	//this->codec->fromUnicode(file->clientFileName().c_str()).toStdString()
	std::ofstream dest(saveName.c_str(), std::ios::binary);
	if (dest.fail()) {
		std::cerr << "**** ERROR: The output file could not be opened"
			<< std::endl;
		return;
	}

	dest << src.rdbuf();

	if (icons_.find(file) != icons_.end()) {
		icons_[file]->addStyleClass("ready");
		drop_->remove(file);
	}
}

void ShareAnyWebApplication::updateProgressListener()
{
	// if there is a next file listen for progress
	if (drop_->currentIndex() < drop_->uploads().size()) {
		Wt::WFileDropWidget::File* file = drop_->uploads()[drop_->currentIndex()];
        file->dataReceived().connect(this, &ShareAnyWebApplication::showProgress);
		std::string fileName = Wt::Utils::htmlEncode(file->clientFileName());
		log_->setText("uploading file &quot;" + fileName + "&quot;");
	}
}

void ShareAnyWebApplication::showProgress(::uint64_t current, ::uint64_t total)
{
	Wt::WFileDropWidget::File* file = drop_->uploads()[drop_->currentIndex()];
	std::string c = std::to_string(current / 1024);
	std::string t = std::to_string(total / 1024);
	std::string fileName = Wt::Utils::htmlEncode(file->clientFileName());
	log_->setText("uploading file <i>&quot;" + fileName + "&quot;</i>"
		+ " (" + c + "kB" + " out of " + t + "kB)");
}
