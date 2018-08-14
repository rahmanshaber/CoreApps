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

#ifndef CORESHOT_H
#define CORESHOT_H

#include "coreshot/coreshotdialog.h"

#include <QFileDialog>
#include <QWidget>
#include <QPixmap>

#include "utilities/utilities.h"
#include "utilities/globalfunctions.h"
#include "settings/settingsmanage.h"


namespace Ui {
class coreshot;
}

class coreshot : public QWidget
{
    Q_OBJECT

public:
    explicit coreshot(QWidget *parent = nullptr);
    ~coreshot();

    void setPixmap(QPixmap &pix);

private slots:
    void on_openInCorePaint_clicked();
    void on_save_clicked();
    void on_saveAs_clicked();
    void on_cancel_clicked();
    void on_newShot_clicked();
    void on_openInCoreImage_clicked();

private:
    Ui::coreshot *ui;

    SettingsManage sm;
    QString files;

};

#endif // CORESHOT_H
