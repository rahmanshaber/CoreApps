/*
CoreBox is combination of some common desktop apps.

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

// ======================== Recent Activity =============================
QString sentDateText(const QString &dateTime)
{
    QDateTime given = QDateTime::fromString(dateTime, "dd.MM.yyyy");
    if (QDate::currentDate().toString("dd.MM.yyyy") == dateTime) {
        return QString("Today");
    } else {
        return QString(given.toString("MMMM dd"));
    }
}

bool checkRecentActivityFile()
{
    QFile file(QDir::homePath() + "/.config/coreBox/RecentActivity");
    if (file.exists()) {
        return true;
    }
    return false;
}

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

// =========================================================================

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


void openAppEngine(const QString &path) // engine send right file to coreapps or system
{
    QFileInfo info(path);
    if(!info.exists() && !path.isEmpty()){
        // Function from globalfunctions.cpp
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
        appEngine(CoreFM, info.absoluteFilePath());
        return;
    }

    //CoreImage
    else if (image.contains(suffix, Qt::CaseInsensitive)) {
        appEngine(CoreImage, info.absoluteFilePath());
        return;
    }

    //CorePad
    else if (txts.contains(suffix, Qt::CaseInsensitive)) {
        appEngine(CorePad, info.absoluteFilePath());
        return;
    }

    //CorePDF
    else if (pdf.contains(suffix, Qt::CaseInsensitive)) {
        appEngine(CorePDF, info.absoluteFilePath());
        return;
    }

    //sendtomimeutils
    else {
//        MimeUtils *mimeUtils;
//        mimeUtils = new MimeUtils(this);
//        mimeUtils->openInApp(QFileInfo(s),this);
    }
}

QRect screensize() // gives the system screen size
{
    QScreen * screen = QGuiApplication::primaryScreen();
    return screen->availableGeometry();
}

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
