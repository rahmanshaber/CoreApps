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

#ifndef UTILITIES_H
#define UTILITIES_H

#include "settings/settingsmanage.h"


enum MessageType {
    Info = 0,
    Warning = 1,
    Tips = 2
};

enum sortOrder {
    ASCENDING = 0,
    DESCENDING = 1
};

enum FolderSetup {
    BookmarkFolder = 0,
    DriveMountFolder = 1,
    TrashFolder
};

    QString checkIsValidDir(const QString str);
    QString checkIsValidFile(const QString str);

    QString getStylesheetFileContent(const QString &path);
    QString readStringFromFile(const QString &path, const QIODevice::OpenMode &mode);
    QSettings *getStylesheetValue();

    bool moveToTrash(const QString &fileName);
    void messageEngine(const QString &message, MessageType messageType);

    QStringList fStringList(QStringList left, QStringList right, QFont font);

    QRect screensize();

    QString formatSize(qint64 num);

    QString sentDateText(const QString &dateTime);
    bool saveToRecent(QString appName, const QString &pathName);

    void setupFolder(FolderSetup fs);

    QIcon getAppIcon(const QString &appName);
    QIcon getFileIcon(const QString &filePath);

    #include <QStyle>
    #include <QGraphicsDropShadowEffect>
    #include <QDesktopWidget>
    static void addDropShadow(QWidget *widget, const int alpha, const int blur = 25, const QString stylesheet = "") {
        SettingsManage sm;
        if (sm.getAddShadow()) {
             QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(widget);
             effect->setBlurRadius(blur);
             effect->setColor(QColor(0, 0, 0, alpha));
             effect->setOffset(0);
             widget->setGraphicsEffect(effect);
             if (!stylesheet.isNull())
                 widget->setStyleSheet(stylesheet);
        }
    }


    QStringList sortDate(QStringList &dateList, sortOrder s = ASCENDING);
    QStringList sortTime(QStringList &timeList, sortOrder s = ASCENDING, QString format = "hh.mm.ss");
    QStringList sortList(QStringList &list, sortOrder s = ASCENDING);
    QStringList sortDateTime(QStringList &dateTimeList, sortOrder s = ASCENDING);



#endif // UTILITIES_H
