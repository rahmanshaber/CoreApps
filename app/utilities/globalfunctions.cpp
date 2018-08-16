/*
CoreBox is combination of some common desktop apps

CoreBox is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see {http://www.gnu.org/licenses/}. */

#include "globalfunctions.h"

#include "corepad/corepad.h"
#include "start/start.h"
#include "settings/settings.h"
#include "start/sessionsavedialog.h"
#include "about/about.h"
#include "help/help.h"
#include "search/search.h"
#include "dashboard/dashboard.h"
#include "corefm/corefm.h"
#include "corepaint/corepaint.h"
#include "coreimage/coreimage.h"
#include "coreplayer/coreplayer.h"
#include "coreaction/coreaction.h"
#include "coretime/coretime.h"
#include "corepdf/corepdf.h"
#include "coreterminal/coreterminal.h"
#include "corerenamer/corerenamer.h"



//bool checkRecentActivityFile()
//{
//    QFile file(QDir::homePath() + "/.config/coreBox/RecentActivity");
//    if (file.exists()) {
//        return true;
//    }
//    return false;
//}



// =========================================================================
#include <QString>
#include <QStringList>
#include <settings/settingsmanage.h>
void appEngine(Category ctg ,const QString path)
{
    SettingsManage sm;

    switch (ctg) {

    case FileManager: {

        QString defultFileManager = sm.getFileManager(); // selected FileManager name from settings.

        QProcess::startDetached(defultFileManager, QStringList() << path);

        // Function from utilities.cpp
        QString mess = defultFileManager + " opening " ;
        messageEngine(mess, MessageType::Info);

        break;
    }
    case TextEditor: {

        QString defultTextEditor = sm.getTextEditor(); // selected TextEditor name from settings.

        QProcess::startDetached(defultTextEditor, QStringList() << path);

        // Function from utilities.cpp
        QString mess = defultTextEditor + " opening " ;
        messageEngine(mess, MessageType::Info);

        break;
    }
    case ImageViewer: {

        QString defultImageViewer = sm.getImageViewer(); // selected ImageViewer name from settings.

        QProcess::startDetached(defultImageViewer, QStringList() << path);

        // Function from utilities.cpp
        QString mess = defultImageViewer + " opening " ;
        messageEngine(mess, MessageType::Info);

        break;
    }
    case Terminal: {

        QString defultTerminal = sm.getTerminal(); // selected terminal name from settings.
        QStringList args(defultTerminal.split(" "));
        QString name = args.at(0);
        args.removeAt(0);

        if (name == "CoreTerminal") {
            appEngines(CoreTerminal,path);
        } else {
            QProcess::startDetached(name, args, path);
        }

        // Function from utilities.cpp
        QString mess = defultTerminal + " opening " ;
        messageEngine(mess, MessageType::Info);

        break;
    }

    }
}


void appEngines(AppsName i,const QString arg) // engine to open app in window
{
    switch (i) {
    case CoreFM: {
        corefm *app = new corefm();
        QString str = checkIsValidDir(arg);
        if (!str.isEmpty() || !str.isNull()) app->goTo(str);
        app->show();

        break;
    }
    case CoreImage: {
        coreimage *app = new coreimage();
        QString str = checkIsValidFile(arg);
        if (!str.isEmpty() || !str.isNull()) app->loadFile(str);
        app->show();

        break;
    }
    case CorePad: {
        corepad *app = new corepad();
        app->openText(checkIsValidFile(arg));
        app->show();

        break;
    }
    case CorePaint: {
        corepaint *app = new corepaint();
        const QString str = checkIsValidFile(arg);
        if (!str.isEmpty() || !str.isNull()) app->initializeNewTab(true, str);
        else app->initializeNewTab();
        app->show();

        break;
    }
    case CorePlayer: {
        coreplayer *app = new coreplayer();
        const QString str = checkIsValidFile(arg);
        if (!str.isEmpty() || !str.isNull()) app->openPlayer(str);
        app->show();

        break;
    }
    case Dashboard: {
        dashboard *app = new dashboard();
        app->show();

        break;
    }
    case Bookmarks: {
        bookmarks *app = new bookmarks();
        app->show();

        break;
    }
    case About: {
        about *app = new about();
        app->show();

        break;
    }
    case StartView: {
        Start *app = new Start();
        app->show();

        break;
    }
    case Help: {
        help *app = new help();
        app->show();

        break;
    }
    case Settings: {
        settings *app = new settings();
        app->show();

        break;
    }
    case Search: {
        search *app = new search();
        QString str = checkIsValidDir(arg);
        if (!str.isEmpty() || !str.isNull()) app->setPath(str);
        app->show();

        break;
    }
    case CoreTime: {
        coretime *app = new coretime();
        app->show();

        break;
    }
    case CorePDF: {
        corepdf *app = new corepdf();
        const QString str = checkIsValidFile(arg);
        if (!str.isEmpty() || !str.isNull()) app->openPdfFile(str);
        app->show();

        break;
    }
    case CoreTerminal: {
        QString workDir = arg;
        if (!arg.count())
            workDir = QDir::homePath();
        if (QFileInfo(workDir).isFile())
            workDir = QFileInfo(arg).path();
//        coreterminal *app = new coreterminal(workDir, "", this);
//        app->show();

        break;
    }
    case CoreRenamer: {
        corerenamer *app = new corerenamer();
        const QString str = checkIsValidDir(arg);
        if (!str.isEmpty() || !str.isNull()) app->addPath(str);
        app->show();

        break;
    }
    default:
        break;
    }
}


void appSelectionEngine(const QString &path) // engine send right file to coreapps or system
{
    QFileInfo info(path);
    if(!info.exists() && !path.isEmpty()){
        // Function from utilities.cpp
        messageEngine("File not exists", MessageType::Warning);
        return;
    }

    QStringList image, txts,pdf;
    image << "jpg" << "jpeg" << "png" << "bmp" << "ico" << "svg" << "gif";
    txts << "txt" << "pro" << "";
    pdf << "pdf";

    QString suffix = QFileInfo(path).suffix();

    //CoreFM
    if (info.isDir()) {
        appEngine(FileManager, info.absoluteFilePath());
        return;
    }

    //CoreImage
    else if (image.contains(suffix, Qt::CaseInsensitive)) {
        appEngine(ImageViewer, info.absoluteFilePath());
        return;
    }

    //CorePad
    else if (txts.contains(suffix, Qt::CaseInsensitive)) {
        appEngine(TextEditor, info.absoluteFilePath());
        return;
    }

    //CorePDF
    else if (pdf.contains(suffix, Qt::CaseInsensitive)) {
        appEngines(CorePDF, info.absoluteFilePath());
        return;
    }

    //sendtomimeutils
    else {
//        MimeUtils *mimeUtils;
//        mimeUtils = new MimeUtils(this);
//        mimeUtils->openInApp(QFileInfo(s),this);
    }
}











