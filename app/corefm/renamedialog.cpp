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

#include "renamedialog.h"
#include "ui_renamedialog.h"


renameDialog::renameDialog(QFileInfo iFile,QWidget *parent):QDialog(parent),ui(new Ui::renameDialog)
{
    ui->setupUi(this);

    // set stylesheet from style.qrc
    setStyleSheet(Utilities::getStylesheetFileContent(":/appStyle/style/Dialog.qss"));

    m_iFile = iFile;
    setWindowFlags(Qt::Dialog | Qt::Popup);
    setAttribute(Qt::WA_NativeWindow);
    setWindowIcon(QIcon(":/app/icons/app-icons/CoreFM.svg"));
    setWindowTitle("Rename \"" + m_iFile.fileName() + "\"");
    ui->newName->setText(m_iFile.fileName());
    connect(ui->cancel, &QToolButton::clicked, this, &renameDialog::close);

    shotcuts();
}

renameDialog::~renameDialog()
{
    delete ui;
}

void renameDialog::shotcuts()
{
    QShortcut* shortcut;

    shortcut = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    connect(shortcut, &QShortcut::activated, this, &renameDialog::on_done_clicked);
}

void renameDialog::on_done_clicked()
{
    if (ui->newName->text().count()) {
        if (QFile::rename(m_iFile.filePath(), m_iFile.path() + "/" + ui->newName->text())) {
            Utilities::messageEngine("File Renamed Successfully.", Utilities::MessageType::Info);
            close();
        } else {
            Utilities::messageEngine("File not Renamed.", Utilities::MessageType::Warning);
            close();
        }
    }
}

void renameDialog::on_newName_returnPressed()
{
    on_done_clicked();
}
