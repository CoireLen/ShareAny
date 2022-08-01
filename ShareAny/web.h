#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WFileResource.h>
#include <Wt/WFileDropWidget.h>
#include <Wt/WProgressBar.h>
#include <Wt/WResource.h>
#include <Wt/WLink.h>
#include <Wt/WPanel.h>
#include <Wt/WTemplate.h>
#include <Wt/WAudio.h>
#include <Wt/WImage.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVideo.h>
#include <Wt/WJavaScript.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WServer.h>
#include "Wt/Utils.h"
#include <QtCore/qstring.h>
#include <QtCore/qtextcodec.h>
#include <fstream>

class ShareAnyWebApplication : public Wt::WApplication
{
public:
    ShareAnyWebApplication(const Wt::WEnvironment& env, std::vector<std::pair<QString, QByteArray>>* dataList,QString upFolder,bool useupload);
private:
    std::vector<std::pair<QString, QByteArray>>* dataList;
    unsigned int id = 0;
    QString uploadFolder;//���ش���ļ��A
    unsigned int maxfiles = 36;
    Wt::WText* log_;
    Wt::WFileDropWidget* drop_;
    int nbUploads_=0;
    std::map<Wt::WFileDropWidget::File*, Wt::WContainerWidget*> icons_;
    void handleDrop(std::vector<Wt::WFileDropWidget::File*> files);
    void tooLarge(Wt::WFileDropWidget::File* file, ::uint64_t);
    void failed(Wt::WFileDropWidget::File* file);
    void saveFile(Wt::WFileDropWidget::File* file);
    void cancelUpload();
    void updateProgressListener();
    void showProgress(::uint64_t current, ::uint64_t total);
};
//����
