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

    QIcon geticon(const QString &filePath);
    QString formatSize(qint64 num);

    QString sentDateText(const QString &dateTime);
    bool saveToRecent(QString appName, const QString &pathName);

    void setupFolder(FolderSetup fs);

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
