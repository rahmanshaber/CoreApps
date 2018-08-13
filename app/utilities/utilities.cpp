/*
CoreBox give's a file's detail information.

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

#include "utilities.h"

#include <QApplication>
#include <QScreen>
QRect screensize() // gives the system screen size
{
    QScreen * screen = QGuiApplication::primaryScreen();
    return screen->availableGeometry();
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


bool moveToTrash(const QString &fileName) // moves a file or folder to trash folder
{
//    if (getSize(fileName) >= 1073741824) {
//        QMessageBox::StandardButton replyC;
//        replyC = QMessageBox::warning(qApp->activeWindow(), "Warning!", "File size is about 1 GB or larger.\nPlease delete it instead of moveing to trash.\nDo you want to delete it?", QMessageBox::Yes | QMessageBox::No);
//        if (replyC == QMessageBox::No) {
//            return false;

//        } else {
//            QFile::remove(fileName);
//            return true;
//        }
//    }
//    else {
//        // Check the trash folder for it't existence
//        setupFolder(TrashFolder);

//        QDir trash(QDir::homePath() + "/.local/share/Trash");
//        QFile directorySizes(trash.path() + "/directorysizes");
//        directorySizes.open(QFile::Append);

//        QMessageBox::StandardButton reply;
//        reply = QMessageBox::warning(qApp->activeWindow(), "Warning!", "Do you want to Trash the '" + fileName + "' ?", QMessageBox::Yes | QMessageBox::No);
//        if (reply == QMessageBox::Yes) {
//            QString fileLocation = fileName;
//            if (QFile(fileLocation).exists()) {
//                QFile(fileLocation).rename(trash.path() + "/files/" + QFileInfo(fileName).fileName());
//            } else {
//                QDir(QFileInfo(fileName).path()).rename(QFileInfo(fileName).fileName(), trash.path() + "/files/ " + QFileInfo(fileName).fileName());
//            }
//            QFile trashinfo(trash.path() + "/info/" + QFileInfo(fileName).fileName() + ".trashinfo");
//            trashinfo.open(QFile::WriteOnly);
//            trashinfo.write(QString("[Trash Info]\n").toUtf8());
//            trashinfo.write(QString("Path=" + fileLocation + "\n").toUtf8());
//            trashinfo.write(QString("DeletionDate=" + QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss") + "\n").toUtf8());trashinfo.close();

//            // Function from globalfunctions.cpp
//            messageEngine("File Moved to Trash", MessageType::Info);
//            return true;
//        }
//    }
//    return false;
}

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
void messageEngine(const QString &message, MessageType messageType) // engine show any message with type in desktop corner
{
    QLabel *l = new QLabel(message);
    QFont f ("Arial", 14, QFont::Bold);
    QWidget *mbox = new QWidget();
    QVBoxLayout *bi = new QVBoxLayout();
    QVBoxLayout *bii = new QVBoxLayout();
    QFrame *frame = new QFrame();
    frame->setStyleSheet("QFrame { border-radius: 5px; }");
    bii->addWidget(frame);
    frame->setLayout(bi);
    mbox->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::ToolTip);
    mbox->setAttribute(Qt::WA_TranslucentBackground);
    mbox->setMinimumSize(230, 50);
    mbox->setLayout(bii);
    l->setStyleSheet("QLabel { padding: 10px; }");
    l->setFont(f);
    l->setAlignment(Qt::AlignCenter);
    bi->addWidget(l);
    bi->setContentsMargins(0, 0, 0, 0);
    QString stylesheet;
    if (messageType == MessageType::Info) {
        stylesheet = "QWidget { background-color: rgba(35, 35, 35, 200); color: #ffffff; border: 1px #2A2A2A; border-radius: 3px; }";
    } else if (messageType == MessageType::Warning) {
        stylesheet = "QWidget { background-color: rgba(240, 0, 0, 150); color: #ffffff; border: 1px #2A2A2A; border-radius: 3px; }";
    } else if (messageType == MessageType::Tips) {
        stylesheet = "QWidget { background-color: rgba(0, 0, 240, 150); color: #ffffff; border: 1px #2A2A2A; border-radius: 3px; }";
    } else {
        return;
    }

    mbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    addDropShadow(mbox, 60);
    mbox->setStyleSheet(stylesheet);
    mbox->show();

    int x = screensize().width() - (mbox->width() + 5);
    int y = screensize().height() - (mbox->height() + 5);
    mbox->move(x,y);

    QTimer::singleShot(3000, mbox, SLOT(close()));

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

QString formatSize(qint64 num) // separete size in universal size format
{
    QString total;
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;

    if (num >= tb) total = QString("%1TB").arg(QString::number(qreal(num) / tb, 'f', 2));
    else if(num >= gb) total = QString("%1GB").arg(QString::number(qreal(num) / gb, 'f', 2));
    else if(num >= mb) total = QString("%1MB").arg(QString::number(qreal(num) / mb, 'f', 1));
    else if(num >= kb) total = QString("%1KB").arg(QString::number(qreal(num) / kb,'f', 1));
    else total = QString("%1 bytes").arg(num);

    return total;
}


#include <QFont>
#include <QFontMetrics>
QStringList fStringList(QStringList left, QStringList right, QFont font) // add two stringlist with ":"
{
    QFontMetrics *fm = new QFontMetrics(font);
    int large = 0;

    for (int i = 0; i < left.count(); i++) {
        if (large < fm->width(left.at(i))) {
            large = fm->width(left.at(i));
        }
    }

    large = large + fm->width('\t');

    for (int i = 0; i < left.count(); i++) {
        while (large >= fm->width(left.at(i))) {
             left.replace(i, QString(left.at(i) + QString('\t')));
        }
    }

    for (int i = 0; i < left.count(); i++) {
        QString total = left.at(i);
        QString firstWoard = total.at(0).toUpper();
        QString otherWord = total.right(total.length() - 1 );
        QString s = left.at(i);
        left.replace(i, firstWoard + otherWord + ": " + right.at(i));
    }

    return left;
}


QString readStringFromFile(const QString &path, const QIODevice::OpenMode &mode)
{
    QSharedPointer<QFile> file(new QFile(path));
    QString data;

    if(file->open(mode)) {
      data = file->readAll();
      file->close();
    }

    return data;
}


QString getStylesheetFileContent(const QString &path)
{
    SettingsManage sm;
    QSettings *mStyleValues;
    QString mStylesheetFileContent;
    QString appThemePath;

    // Load view mode
    if (sm.getStyleMode()) {
        appThemePath = ":/theme/style/modeLight.ini";
    }else {
        appThemePath = ":/theme/style/modeDark.ini";
    }

    mStyleValues = new QSettings(appThemePath, QSettings::IniFormat);
    mStylesheetFileContent = readStringFromFile(QString(path),QIODevice::ReadOnly);

    // set values example: @color01 => #fff
    for (const QString &key : mStyleValues->allKeys()) {
        mStylesheetFileContent.replace(key, mStyleValues->value(key).toString());
    }

    return mStylesheetFileContent;

//    @color01=apps mainWidget Color
//    @color02=dialogBox Color
//    @color03=apps topBar Color
//    @color04=highLight Color
//    @color05=border Color
//    @color06=apps seconderyWidget Color
//    @color07=apps text Color
//    @color08=apps HightlightText color
//    @color09=apps sidebarText color
}

QSettings *getStylesheetValue()
{
    SettingsManage sm;
    QSettings *mStyleValues;
    QString appThemePath;

    // Load view mode
    if (sm.getStyleMode()) {
        appThemePath = ":/theme/style/modeLight.ini";
    }else {
        appThemePath = ":/theme/style/modeDark.ini";
    }

    mStyleValues = new QSettings(appThemePath, QSettings::IniFormat);

    return mStyleValues;
}

void setupFolder(FolderSetup fs)
{
    switch (fs) {
    case FolderSetup::BookmarkFolder: {
        // Setup corebox folder for bookmarks
        const QString b = QDir::homePath() + ".config/coreBox";
        if (!QDir(b).exists()) {
            QDir::home().mkdir(".config/coreBox");
        }
        break;
    }
    case FolderSetup::DriveMountFolder: {
        // Setup drive mount folder
        const QString d = QDir::homePath() + "/.coreBox";
        if(!QDir(d).exists()) {
            QDir::home().mkdir(".coreBox");
        }
        break;
    }
    case FolderSetup::TrashFolder: {
        // Setup trash folder
        const QString t = QDir::homePath() + ".local/share/Trash";
        if (!QDir(t).exists()) {
            QDir trash = QDir::home();
            trash.cd(".local/share/");
            trash.mkdir("Trash");
            trash.cd("Trash");
            trash.mkdir("files");
            trash.mkdir("info");
        }
        break;
    }
    default:
        break;
    }
}

#include <QMimeDatabase>
#include <QMimeType>
QIcon geticon(const QString &filePath) // gives a file or folder icon from system
{
    QIcon icon;
    QFileInfo info(filePath);

    QMimeDatabase mime;
    QMimeType mType;

    mType = mime.mimeTypeForFile(filePath);
    icon = QIcon::fromTheme(mType.iconName());

    if (icon.isNull())
        return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    else
        return icon;
}

// ======================== Recent Activity =============================
#include <QDateTime>
QString sentDateText(const QString &dateTime)
{
    QDateTime given = QDateTime::fromString(dateTime, "dd.MM.yyyy");
    if (QDate::currentDate().toString("dd.MM.yyyy") == dateTime) {
        return QString("Today");
    } else {
        return QString(given.toString("MMMM dd"));
    }
}

#include <QDateTime>
bool saveToRecent(QString appName, const QString &pathName) // save file path and app name for recent activites
{
    SettingsManage sm;
    if (sm.getDisableRecent() == false) {
        if (appName.count() && pathName.count()) {
            QSettings recentActivity(QDir::homePath() + "/.config/coreBox/RecentActivity", QSettings::IniFormat);
            QDateTime currentDT = QDateTime::currentDateTime();
            QString group = currentDT.toString("dd.MM.yyyy");
            QString key = currentDT.toString("hh.mm.ss");
            recentActivity.beginGroup(group);
            recentActivity.setValue(key, appName + "\t\t\t" + pathName);
            recentActivity.endGroup();
            return true;
        }
    }
    return false;
}



QStringList sortDate(QStringList &dateList, sortOrder s)
{
    QList<QDate> dates;

    foreach (QString str, dateList) {
        dates.append(QDate::fromString(str, "dd.MM.yyyy"));
    }

    std::sort(std::begin(dates), std::end(dates));

    if (s == ASCENDING) {
        for (int i = 0; i < dateList.count(); i++) {
            dateList.replace(i, dates[i].toString("dd.MM.yyyy"));
        }
    } else {
        int reverse = dateList.count() - 1;
        for (int i = 0; i < dateList.count(); i++) {
            dateList.replace(reverse, dates[i].toString("dd.MM.yyyy"));
            reverse--;
        }
    }

    dates.clear();
    return dateList;
}

QStringList sortTime(QStringList &timeList, sortOrder s, QString format)
{
    QList<QTime> times;

    foreach (QString str, timeList) {
        times.append(QTime::fromString(str, format));
    }

    std::sort(std::begin(times), std::end(times));

    if (s == ASCENDING) {
        for (int i = 0; i < timeList.count(); i++) {
            timeList.replace(i, times[i].toString(format));
        }
    } else {
        int reverse = timeList.count() - 1;
        for (int i = 0; i < timeList.count(); i++) {
            timeList.replace(reverse, times[i].toString(format));
            reverse--;
        }
    }

    times.clear();
    return timeList;
}

#include <QCollator>
QStringList sortList(QStringList &list, sortOrder s)
{
    QCollator sortNum;
    sortNum.setNumericMode(true);

    if (s == ASCENDING) {
        std::sort(list.begin(), list.end(), sortNum);
    } else {
        std::sort(list.begin(), list.end(), [&sortNum](const QString &s1, const QString &s2) {
            return sortNum.compare(s1, s2) > 0;
        });
    }

    return list;
}

QStringList sortDateTime(QStringList &dateTimeList, sortOrder s)
{
    QList<QDateTime> dts;

    foreach (QString str, dateTimeList) {
        dts.append(QDateTime::fromString(str, "hh.mm.ss - dd.MM.yyyy"));
    }

    std::sort(std::begin(dts), std::end(dts));

    if (s == ASCENDING) {
        for (int i = 0; i < dateTimeList.count(); i++) {
            dateTimeList.replace(i, dts[i].toString("hh.mm.ss - dd.MM.yyyy"));
        }
    } else {
        int reverse = dateTimeList.count() - 1;
        for (int i = 0; i < dateTimeList.count(); i++) {
            dateTimeList.replace(reverse, dts[i].toString("hh.mm.ss - dd.MM.yyyy"));
            reverse--;
        }
    }

    dts.clear();
    return dateTimeList;
}


