#include "utilities.h"

#include "start/start.h"
#include "settings/settings.h"
#include "start/sessionsavedialog.h"
#include "about/about.h"
#include "help/help.h"
#include "search/search.h"
#include "dashboard/dashboard.h"
#include "corepad/corepad.h"
#include "corefm/corefm.h"
#include "corepaint/corepaint.h"
#include "coreimage/coreimage.h"
#include "coreplayer/coreplayer.h"
#include "coreaction/coreaction.h"
#include "coretime/coretime.h"
#include "corepdf/corepdf.h"
#include "coreterminal/coreterminal.h"
#include "corerenamer/corerenamer.h"

void utilities()
{
    SettingsManage sm;
    // set a icon set for the whole app
    QIcon::setThemeName(sm.getThemeName());

    // set one font style across all the apps
    QFont fl (sm.getFontStyle(), 10, QFont::Normal);
    QApplication::setFont(fl);

    setupFolder(FolderSetup::BookmarkFolder);
    setupFolder(FolderSetup::DriveMountFolder);
    setupFolder(FolderSetup::TrashFolder);
}


void appEngine(AppsName i,const QString arg) // engine to open app in window
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

QString checkIsValidDir(QString str) // cheack if a folder/dir is valid
{
    if (str.isEmpty() || str.isNull()) {
        return "";
    } else {
        QFileInfo dir(str);
        if (dir.isDir()) {
            if (dir.isRoot()) return str;
            else {
                if (str.endsWith('/')) return str.remove(str.length() - 1, 1);
                else return str;
            }
        }
    }
    return "";
}

QString checkIsValidFile(const QString str) // cheack if a file is valid
{
    if (str.isEmpty() || str.isNull()) {
        return "";
    } else {
        QFileInfo fi(str);
        if (fi.isFile()) {
            return str;
        }
    }
    return "";
}

//void utilities::reload(int index) // reload the apps if related app is clicked
//{
//    QString appName = ui->windows->tabBar()->tabText(index);

//    if (appName == "Bookmarks") {
//        bookmarks *cbook = ui->windows->findChild<bookmarks*>("bookmarks");
//        cbook->reload();
//    } else if (appName == "Start") {
//        Start *cstart = ui->windows->findChild<Start*>("Start");
//        cstart->reload();
//    }
//}


//===========================WindowBar========End=================================================================


//void utilities::on_saveSession_clicked()
//{
//    QString sName = "";
//    sessionSaveDialog *ssd = new sessionSaveDialog(this);
//    connect(ssd, &sessionSaveDialog::nameOk, [this, ssd, &sName]() {
//        sName = ssd->sName;
//        ssd->close();
//    });
//    ssd->exec();

//    if (!sName.count()) {
//        messageEngine("Session Name empty\nNot Saved", MessageType::Info);
//        return;
//    }

//    QSettings session(QDir::homePath() + "/.config/coreBox/Sessions", QSettings::IniFormat);

//    if (session.childGroups().count() > 15) {
//        messageEngine("Session save limit reached", MessageType::Warning);
//        return;
//    }

//    session.beginGroup(QDate::currentDate().toString("dd.MM.yyyy"));
//    session.beginGroup(sName);
//    for (int i = 0; i < ui->windows->count(); i++) {
//        QThread::currentThread()->msleep(1);
//        QString key = QTime::currentTime().toString("hh.mm.ss.zzz");
//        QString value = "";
//        switch (nameToInt(ui->windows->tabText(i))) {
//        case CoreImage: {
//            session.beginGroup("CoreImage");
//            value = static_cast<coreimage*>(ui->windows->widget(i))->currentImagePath;
//            session.setValue(key, value);
//            session.endGroup();
//            break;
//        }
//        case CorePDF: {
//            session.beginGroup("CorePDF");
//            value = static_cast<corepdf*>(ui->windows->widget(i))->workFilePath;
//            session.setValue(key, value);
//            session.endGroup();
//            break;
//        }
//        case CorePlayer: {
//            session.beginGroup("CorePlayer");
//            value = static_cast<coreplayer*>(ui->windows->widget(i))->workFilePath();
//            session.setValue(key, value);
//            session.endGroup();
//            break;
//        }
//        case CoreRenamer: {
//            session.beginGroup("CoreRenamer");
//            value = static_cast<corerenamer*>(ui->windows->widget(i))->workFilePath;
//            session.setValue(key, value);
//            session.endGroup();
//            break;
//        }
//        case CoreTerminal: {
//            session.beginGroup("CoreTerminal");
//            value = static_cast<coreterminal*>(ui->windows->widget(i))->currentWorkingDirectory();
//            session.setValue(key, value);
//            session.endGroup();
//            break;
//        }
//        case Search: {
//            session.beginGroup("Search");
//            value = static_cast<search*>(ui->windows->widget(i))->workFilePath();
//            session.setValue(key, value);
//            session.endGroup();
//            break;
//        }
//        case CoreFM: {
//            session.beginGroup("CoreFM");
//            corefm *cfm = static_cast<corefm*>(ui->windows->widget(i));
//            for (int j = 0; j < cfm->tabsCount(); j++){
//                QThread::currentThread()->msleep(1);
//                // Created the key again for the time for every single page
//                key = QTime::currentTime().toString("hh.mm.ss.zzz");

//                // As corefm has so many current path variable (don't know but guess)
//                // Just added a function to get the current page path
//                value = cfm->gCurrentPath(j);
//                session.setValue(key, value);
//            }
//            session.endGroup();
//            break;
//        }
//        case CorePad: {
//            session.beginGroup("CorePad");
//            corepad *cpad = static_cast<corepad*>(ui->windows->widget(i));
//            for (int j = 0; j < cpad->tabsCount(); j++) {
//                QThread::currentThread()->msleep(1);
//                // Created the key again for the time for every single page
//                key = QTime::currentTime().toString("hh.mm.ss.zzz");
//                value = cpad->currentFilePath(j);
//                session.setValue(key, value);
//            }
//            session.endGroup();
//            break;
//        }
//        case CorePaint: {
//            session.beginGroup("CorePaint");
//            corepaint *cpaint = static_cast<corepaint*>(ui->windows->widget(i));
//            for (int j = 0; j < cpaint->tabsCount(); j++) {
//                QThread::currentThread()->msleep(1);
//                // Created the key again for the time for every single page
//                key = QTime::currentTime().toString("hh.mm.ss.zzz");
//                value = cpaint->getImageAreaByIndex(j)->mFilePath;
//                session.setValue(key, value);
//            }
//            session.endGroup();
//            break;
//        }
//        default:
//            session.beginGroup(ui->windows->tabText(i));
//            value = "";
//            session.setValue(key, value);
//            session.endGroup();
//            break;
//        }
//    }
//    session.endGroup();
//    session.endGroup();

//    messageEngine("Session Saved Successfully", MessageType::Info);
//}

