#include "web.h"

/*
建立web服务
展示页面
*/
ShareAnyApplication::ShareAnyApplication(const Wt::WEnvironment& env, std::vector<std::pair<QString, QString>>* dataList,QString upFolder,bool useupload) : WApplication(env)
{
	if (upFolder != "") {
		this->uploadFolder = upFolder;
	}
	this->dataList = dataList;
	setTheme(std::make_shared<Wt::WBootstrap5Theme>());
	setTitle("ShareAny");
	root()->setContentAlignment(Wt::AlignmentFlag::Center);
	//这里放个上传区
	if (useupload) {
		drop_ = root()->addNew<Wt::WFileDropWidget>();

		drop_->setDropIndicationEnabled(true);
		// drop_->setGlobalDropEnabled(true);

		drop_->drop().connect(this, &ShareAnyApplication::handleDrop);
		drop_->newUpload().connect(this, &ShareAnyApplication::updateProgressListener);
		drop_->uploaded().connect(this, &ShareAnyApplication::saveFile);
		drop_->uploadFailed().connect(this, &ShareAnyApplication::failed);
		drop_->tooLarge().connect(this, &ShareAnyApplication::tooLarge);

		log_ = root()->addWidget(std::make_unique<Wt::WText>());
		log_->setInline(false);
		log_->setTextFormat(Wt::TextFormat::XHTML);

		Wt::WPushButton* abort = root()->addNew<Wt::WPushButton>("Abort current upload");
		abort->clicked().connect(this, &ShareAnyApplication::cancelUpload);
	}
	//以下主体动态代码
	
	for (auto i : *dataList) {
		std::cout << i.second.toStdString() << std::endl;
		if (i.first == "text") {
			
			auto container = root()->addNew<Wt::WContainerWidget>();
			container->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			auto hbox = container->setLayout(std::make_unique<Wt::WHBoxLayout>());
			auto input1 = std::make_unique<Wt::WTextArea>(i.second.toStdString());
			input1->setMaximumSize(Wt::WLength("70%"), Wt::WLength(""));
			static Wt::JSlot js= Wt::JSlot("document.getElementById('" + input1->id() + "').select();execCommand('Copy');alert('Copyed');");
			hbox->addWidget(std::move(input1));
			auto pushbutton = std::make_unique<Wt::WPushButton>("COPY");
			pushbutton->clicked().connect(js);
			pushbutton->setMaximumSize(Wt::WLength("30%"), Wt::WLength(""));
			hbox->addWidget(std::move(pushbutton));
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "image") {
			auto textResource = std::make_shared<Wt::WFileResource>(coder->fromUnicode(i.second).toStdString());
			Wt::WLink link = Wt::WLink(textResource);
			link.setTarget(Wt::LinkTarget::Self);
			auto panel = root()->addNew<Wt::WPanel>();
			panel->addStyleClass("centered-example");
			auto img=panel->setCentralWidget(std::make_unique<Wt::WImage>(link));
			//auto img=root()->addNew<Wt::WImage>(link);
			img->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			img->setAlternateText(coder->fromUnicode(i.second).toStdString());
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "audio") {
			auto container = root()->addNew<Wt::WContainerWidget>();
			container->addStyleClass("centered-example");
			container->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			auto hbox = container->setLayout(std::make_unique<Wt::WHBoxLayout>());

			auto filename = i.second.split('/');
			auto textResource = std::make_shared<Wt::WFileResource>(coder->fromUnicode(i.second).toStdString());
			textResource->suggestFileName(filename.at(filename.length() - 1).toStdString());
			Wt::WLink link = Wt::WLink(textResource);
			link.setTarget(Wt::LinkTarget::NewWindow);
			auto musicfile=std::make_unique<Wt::WAnchor>(link, filename.at(filename.length() - 1).toStdString());
			musicfile->setMaximumSize(Wt::WLength("50%"), Wt::WLength("50%"));
			hbox->addWidget(std::move(musicfile));

			auto audio= std::make_unique<Wt::WAudio>();
			audio->addSource(Wt::WLink(link));
			audio->setOptions(Wt::PlayerOption::Controls);
			audio->setMaximumSize(Wt::WLength("50%"), Wt::WLength("50%"));
			audio->setAlternativeContent
			(std::make_unique<Wt::WText>("You don't have HTML5 audio support!"));

			hbox->addWidget(std::move(audio));
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "file") {
			auto textResource = std::make_shared<Wt::WFileResource>("plain/text",coder->fromUnicode(i.second).toStdString());
			auto filename = i.second.split('/');
			textResource->suggestFileName(filename.at(filename.length() - 1).toStdString());
			Wt::WLink link = Wt::WLink(textResource);
			link.setTarget(Wt::LinkTarget::NewWindow);
			auto panel = root()->addNew<Wt::WPanel>();
			panel->addStyleClass("centered-example");
			panel->setCentralWidget(std::make_unique<Wt::WAnchor>(link, filename.at(filename.length() - 1).toStdString()));
			//root()->addNew<Wt::WAnchor>(link,filename.at(filename.length()-1).toStdString());
			root()->addNew<Wt::WBreak>();
		}
		else if (i.first == "video") {
			auto container = root()->addNew<Wt::WContainerWidget>();
			container->addStyleClass("centered-example");
			container->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			auto mp4Video = std::make_shared<Wt::WFileResource>(coder->fromUnicode(i.second).toStdString());
			auto filename = i.second.split('/');
			mp4Video->suggestFileName(filename.at(filename.length() - 1).toStdString());
			auto player = container->addNew<Wt::WVideo>();
			player->addSource(Wt::WLink(mp4Video));
			player->setMaximumSize(Wt::WLength("100%"), Wt::WLength("100%"));
			container->addNew<Wt::WBreak>();
			Wt::WLink link = Wt::WLink(mp4Video);
			link.setTarget(Wt::LinkTarget::NewWindow);
			container->addNew<Wt::WAnchor>(link, filename.at(filename.length() - 1).toStdString());
			root()->addNew<Wt::WBreak>();
		}
	}
	
}

//以下測試
void ShareAnyApplication::handleDrop(std::vector<Wt::WFileDropWidget::File*> files)
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

void ShareAnyApplication::cancelUpload()
{
	if (drop_->uploads().size() == drop_->currentIndex())
		return;

	Wt::WFileDropWidget::File* currentFile = drop_->uploads()[drop_->currentIndex()];
	drop_->cancelUpload(currentFile);
	icons_[currentFile]->addStyleClass("cancelled");
}

void ShareAnyApplication::tooLarge(Wt::WFileDropWidget::File* file, ::uint64_t)
{
	icons_[file]->addStyleClass("invalid");

	log_->setText("File too large: " + file->clientFileName());
}

void ShareAnyApplication::failed(Wt::WFileDropWidget::File* file)
{
	icons_[file]->addStyleClass("invalid");

	log_->setText("Upload failed: " + file->clientFileName());
}

void ShareAnyApplication::saveFile(Wt::WFileDropWidget::File* file)
{
	std::string spool = file->uploadedFile().spoolFileName();
	std::ifstream src(spool.c_str(), std::ios::binary);

	std::string saveName = uploadFolder.toStdString() + file->clientFileName();

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

void ShareAnyApplication::updateProgressListener()
{
	// if there is a next file listen for progress
	if (drop_->currentIndex() < drop_->uploads().size()) {
		Wt::WFileDropWidget::File* file = drop_->uploads()[drop_->currentIndex()];
		file->dataReceived().connect(this, &ShareAnyApplication::showProgress);
		std::string fileName = Wt::Utils::htmlEncode(file->clientFileName());
		log_->setText("uploading file &quot;" + fileName + "&quot;");
	}
}

void ShareAnyApplication::showProgress(::uint64_t current, ::uint64_t total)
{
	Wt::WFileDropWidget::File* file = drop_->uploads()[drop_->currentIndex()];
	std::string c = std::to_string(current / 1024);
	std::string t = std::to_string(total / 1024);
	std::string fileName = Wt::Utils::htmlEncode(file->clientFileName());
	log_->setText("uploading file <i>&quot;" + fileName + "&quot;</i>"
		+ " (" + c + "kB" + " out of " + t + "kB)");
}