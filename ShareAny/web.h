#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WFileResource.h>
#include <Wt/WResource.h>
#include <Wt/WLink.h>
#include <Wt/WPanel.h>
#include <Wt/WTemplate.h>
#include <Wt/WAudio.h>
#include <Wt/WImage.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVideo.h>
#include <Wt/WJavaScript.h>
#include <Wt/WBootstrap5Theme.h>
#include <Wt/WServer.h>
#include <QtCore/qstring.h>
#include <QtCore/qtextcodec.h>

class ShareAnyApplication : public Wt::WApplication
{
public:
    ShareAnyApplication(const Wt::WEnvironment& env, std::vector<std::pair<QString, QString>>* dataList);
    void RandPage(const std::string& internalPath);
private:
    QTextCodec* coder = QTextCodec::codecForName("GBK");
    std::vector<std::pair<QString, QString>>* dataList;
    //QString CopyButton();
};
//≤‚ ‘
