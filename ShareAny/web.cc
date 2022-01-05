#include "web.h"

/*
建立web服务
展示页面
*/
ShareAnyApplication::ShareAnyApplication(const Wt::WEnvironment& env, std::vector<std::pair<QString, QString>>* dataList) : WApplication(env)
{
	this->dataList = dataList;
	Wt::WApplication* app = new Wt::WApplication(env);
	app->setTitle("ShareAnything");
	setTheme(std::make_shared<Wt::WBootstrap5Theme>());
	//setCssTheme("polished");
	//<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
	root()->setContentAlignment(Wt::AlignmentFlag::Center);
	//这里放个上传区D:\Program\ShareAny\upload
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

//以下测试
