#ifndef COREINFO_H
#define COREINFO_H

#include <QWidget>
#include <QDir>
#include <QMimeData>
#include <QTextBrowser>
#include <QTreeWidget>

#include "Core.h"

#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringListList.h>

using namespace ZenLib;

#define wstring2QString(_DATA) \
    QString::fromUtf8(Ztring(_DATA).To_UTF8().c_str())
#define QString2wstring(_DATA) \
    Ztring().From_UTF8(_DATA.toUtf8())

namespace Ui {
class coreinfo;
}

class coreinfo : public QWidget
{
    Q_OBJECT

public:
    explicit coreinfo(QWidget *parent = 0);
    ~coreinfo();

    void sendFiles(const QStringList &paths);

    void openFiles(QStringList fileNames);
    void refreshDisplay();
    QDir getCommonDir(Core *C);

private:
    Ui::coreinfo *ui;
    Core* C;
    QTreeWidget *showTreeView(bool completeDisplay);
    QString shortName(QDir d, QString name);
};

#endif // COREINFO_H
