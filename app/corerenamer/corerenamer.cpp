/*
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

#include "corerenamer.h"
#include "ui_corerenamer.h"


corerenamer::corerenamer(QWidget *parent) :QWidget(parent),ui(new Ui::corerenamer)
{
    ui->setupUi(this);

    // set stylesheet from style.qrc
    setStyleSheet(getStylesheetFileContent(":/appStyle/style/CoreRenamer.qss"));

    // set window size
    int x = static_cast<int>(screensize().width()  * .8);
    int y = static_cast<int>(screensize().height()  * .7);
    this->resize(x, y);

    uStack = new QUndoStack(this);
    ui->addTextCPos->setValidator(new QIntValidator());
    ui->removeTextCPos->setValidator(new QIntValidator());
    ui->removeTextCCount->setValidator(new QIntValidator());

    ui->bUndo->setEnabled(false);
    ui->bRedo->setEnabled(false);

    ui->FLists->setColumnWidth(1,500);

    connect(uStack, &QUndoStack::canUndoChanged, [this](bool canUndo) {
        ui->bUndo->setEnabled(canUndo);
    });
    connect(uStack, &QUndoStack::canRedoChanged, [this](bool canRedo) {
        ui->bRedo->setEnabled(canRedo);
    });

    customSortM = new customSortProxyM();
    m_Model = new QStandardItemModel(0,3);

    addDropShadow(ui->renameTools,40);

    shotcuts();
    startsetup(false);
}

QString treeToText(const QAbstractItemModel* model, const QModelIndex& parent=QModelIndex(), const QString& head = QString()){

    QString result;
    for(int i=0, maxRow = model->rowCount(parent);i<maxRow;++i){
    const QModelIndex currIdx = model->index(i,0,parent);
    result += head + currIdx.data().toString() + '\n';
    if(model->hasChildren(currIdx))
    result += treeToText(model,currIdx,head + "    |");
    }
    result.chop(1);
    return result;
}

corerenamer::~corerenamer()
{
    delete ui;
}

void corerenamer::startsetup(bool set)
{
    ui->renameTools->setEnabled(set);
    ui->bRedo->setEnabled(set);
    ui->bRefreshList->setEnabled(set);
    ui->bUndo->setEnabled(set);
    ui->bRedo->setEnabled(set);
    ui->clearItem->setEnabled(set);
    ui->rename->setEnabled(set);
    ui->FLists->setEnabled(set);
}

void corerenamer::shotcuts()
{
    QShortcut *shortcut;

    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this);
    connect(shortcut, &QShortcut::activated, this, &corerenamer::on_bAddFiles_clicked);

    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), this);
    connect(shortcut, &QShortcut::activated, this, &corerenamer::on_bUndo_clicked);

    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z), this);
    connect(shortcut, &QShortcut::activated, this, &corerenamer::on_bRedo_clicked);
}

QIcon corerenamer::fileIcon(const QString &filePath)
{
    QMimeDatabase mimes;
    QMimeType mime = mimes.mimeTypeForFile(filePath);
    return QIcon::fromTheme(mime.iconName());
}

QString corerenamer::CapitalizeEachWord(const QString &str)
{
    QStringList sep = str.split(' ');
    QString total = "";
    foreach (QString s, sep) {
        if (!s.count())
            continue;

        foreach (QChar c, s) {
            if (c.isLetter()) {
                s = s.toLower().replace(s.indexOf(c), 1, c.toTitleCase());
                break;
            }
        }
        total = total + " " + s;
    }

    return total.remove(0, 1);
}

void corerenamer::addFiles(const QStringList &list)
{
    workFilePath = list.count() ? QFileInfo(list.at(0)).path() : QString();

    m_Model->clear();
    uStack->clear();

    m_Model->setColumnCount(3);
    m_Model->setRowCount(0);

    m_Model->setHeaderData(0, Qt::Horizontal, QObject::tr("Selected"));
    m_Model->setHeaderData(1, Qt::Horizontal, QObject::tr("Changed"));
    m_Model->setHeaderData(2, Qt::Horizontal, QObject::tr("Type"));

    for (int i = 0; i < list.count(); ++i) {
        m_Model->insertRow(0);
        m_Model->setData(m_Model->index(0, 0), fileIcon(list.at(i)).pixmap(24, 24), Qt::DecorationRole);
        m_Model->setData(m_Model->index(0, 0), list.at(i), Qt::UserRole);
        m_Model->setData(m_Model->index(0, 0), QFileInfo(list.at(i)).completeBaseName());
        m_Model->setData(m_Model->index(0, 1), list.at(i), Qt::UserRole);
        m_Model->setData(m_Model->index(0, 1), "");
        m_Model->setData(m_Model->index(0, 2), QFileInfo(list.at(i)).suffix());
    }

    customSortM->setSourceModel(m_Model);
    ui->FLists->setModel(customSortM);
    ui->FLists->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    startsetup(1);
}

void corerenamer::addPath(const QString &path)
{
    QDir dir(path);
    QStringList filePaths;
    foreach (QFileInfo f, dir.entryInfoList(QDir::Files, QDir::Name)) {
        filePaths << f.filePath();
    }
    addFiles(filePaths);
}

QString corerenamer::addText(tPosition tpos, int pos, const QString &newText, const QString &selcText)
{
    int initialPos = 0;
    int addPos = pos;

    if (tpos == FROMRIGHT) {
        initialPos = selcText.length();
        addPos = initialPos - pos;
    }

    return  QString(selcText).insert(addPos, newText);
}

QString corerenamer::remText(tPosition tpos, int pos, int count, const QString &selcText)
{
    int initialPos = 0;
    int remPos = pos;

    if (tpos == FROMRIGHT) {
        initialPos = selcText.length();
        remPos = initialPos - pos - count;
    }

    return  QString(selcText).remove(remPos, count);
}

QString corerenamer::repText(const QString &sText, const QString &rText, const QString &selcText, Qt::CaseSensitivity s)
{
    return QString(selcText).replace(sText, rText, s);
}

void corerenamer::on_bAddFiles_clicked()
{
    QStringList list = QFileDialog::getOpenFileNames(this, "Add files", QDir::currentPath(), "*");
    if (list.count() > 0)
        addFiles(list);
}

bool corerenamer::setAddText()
{
    if (!ui->addTextCPos->text().count())
        return false;

    if (!ui->addTextEnter->text().count())
        return false;

    foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
        if (i.column() == 0) {
            QString sText = m_Model->item(i.row(), 0)->text();
            QString cText = m_Model->item(i.row(), 1)->text();
            if (cText.count())
                sText = cText;
            QString addedT = addText(ui->addFromR->isChecked() ? FROMRIGHT : FROMLEFT, ui->addTextCPos->text().toInt(), ui->addTextEnter->text(), sText);
            uStack->push(new ARRTextCommand(m_Model->item(i.row(), 1), addedT, ADD));
        }
    }

    return true;
}

bool corerenamer::setRemText()
{
    if (!ui->removeTextCPos->text().count())
        return false;

    if (!ui->removeTextCCount->text().count())
        return false;

    foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
        if (i.column() == 0) {
            QString sText = m_Model->item(i.row(), 0)->text();
            QString cText = m_Model->item(i.row(), 1)->text();
            if (cText.count())
                sText = cText;
            QString remedT = remText(ui->remFromR->isChecked() ? FROMRIGHT : FROMLEFT, ui->removeTextCPos->text().toInt(), ui->removeTextCCount->text().toInt(), sText);
            uStack->push(new ARRTextCommand(m_Model->item(i.row(), 1), remedT, REMOVE));
        }
    }

    return true;
}

bool corerenamer::setRepText()
{
    if (!ui->replaceTextText->text().count())
        return false;

    foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
        if (i.column() == 0) {
            QString sText = m_Model->item(i.row(), 0)->text();
            QString cText = m_Model->item(i.row(), 1)->text();
            if (cText.count())
                sText = cText;

            QString replaT = repText(ui->replaceTextText->text(), ui->replaceTextWith->text(), sText);
            uStack->push(new ARRTextCommand(m_Model->item(i.row(), 1), replaT, REPLACE));
        }
    }

    return true;
}

void corerenamer::on_addTextbtn_clicked()
{
    if (ui->FLists->selectionModel()->selectedIndexes().count() < 1)
        return;

    if (setAddText())
        ui->FLists->clearSelection();
}

void corerenamer::on_addTextAllbtn_clicked()
{
    ui->FLists->selectAll();

    if (setAddText())
        ui->FLists->clearSelection();
}

void corerenamer::on_removeTextTextbtn_clicked()
{
    if (ui->FLists->selectionModel()->selectedIndexes().count() < 1)
        return;

    if (setRemText())
        ui->FLists->clearSelection();
}

void corerenamer::on_removeTextAllbtn_clicked()
{
    ui->FLists->selectAll();

    if (setRemText())
        ui->FLists->clearSelection();
}

void corerenamer::on_replaceTextbtn_clicked()
{
    if (ui->FLists->selectionModel()->selectedIndexes().count() < 1)
        return;

    if (setRepText())
        ui->FLists->clearSelection();
}

void corerenamer::on_replaceTextAllbtn_clicked()
{
    ui->FLists->selectAll();

    if (setRepText())
        ui->FLists->clearSelection();
}

void corerenamer::casitivity(int c)
{
    if (ui->FLists->selectionModel()->selectedIndexes().count() < 1)
        return;

    if (ui->addTextCPos->text().isNull())
        return;

    if (ui->addTextEnter->text().isNull())
        return;

    foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
        if (i.column() == 0) {
            QString sText = m_Model->item(i.row(), 0)->text();
            QString cText = m_Model->item(i.row(), 1)->text();
            if (cText.count())
                sText = cText;

            switch (c) {
            case 0:
                sText = sText.toUpper();
                break;
            case 1:
                sText = CapitalizeEachWord(sText);
                break;
            case 2:
                sText = sText.toLower();
                break;
            default:
                sText = sText;
                break;
            }
            uStack->push(new ARRTextCommand(m_Model->item(i.row(), 1), sText, CASE));
        }
    }
}

void corerenamer::on_upperCase_clicked()
{
    casitivity(0);
}

void corerenamer::on_capitalEach_clicked()
{
    casitivity(1);
}

void corerenamer::on_lowerCase_clicked()
{
    casitivity(2);
}

void corerenamer::on_formatc_activated(const QString &arg1)
{
    if (ui->FLists->selectionModel()->selectedIndexes().count() < 1)
        return;

    int count = 0;
    foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
        if (i.column() == 0) {
            QString sText = m_Model->item(i.row(), 0)->text();
            QString cText = m_Model->item(i.row(), 1)->text();
            QString arg = "";
            count++;

            if (cText.count())
                sText = cText;

            if (arg1.at(0) == '0') {
                if (arg1.at(1) == '0') {
                    arg = QString(i.row() > 8 ? i.row() > 98 ? "" : "0" : "00") + QString::number(count) + QString(arg1).remove(0, 3);
                } else {
                    arg = QString(i.row() > 8 ? "" : "0") + QString::number(count) + QString(arg1).remove(0, 2);
                }
            } else arg = QString::number(count) + QString(arg1).remove(0, 1);

            QString numing = addText(ui->numFromR->isChecked() ? FROMRIGHT : FROMLEFT, 0, arg, sText);
            uStack->push(new ARRTextCommand(m_Model->item(i.row(), 1), numing, NUMBER));
        }
    }
}

void corerenamer::on_browseSave_clicked()
{
    ui->savePath->setText(QFileDialog::getSaveFileName(this, "Save file"));
}

QString corerenamer::getOld(QStandardItem *item)
{
    QString ofile = m_Model->item(item->row(), 1)->data(Qt::UserRole).toString();
    if (ui->addOldOnly->isChecked()) {
        ofile = QFileInfo(ofile).fileName();
    }
    return ofile;
}

QString corerenamer::getNew(QStandardItem *item)
{
    QString ofile = m_Model->item(item->row(), 1)->data(Qt::UserRole).toString();
    QString cText = m_Model->item(item->row(), 1)->text();
    if (!cText.count()) return "";
    QString suffix = QFileInfo(ofile).suffix().isNull() ? "" : "." + QFileInfo(ofile).suffix();
    QString nfile = cText.isNull() ? "" : QFileInfo(ofile).path() + "/" + cText + suffix;
    if (ui->addNewOnly->isChecked()) {
        nfile = cText + QFileInfo(ofile).suffix();
    }
    return nfile;
}

void corerenamer::on_saveRenamed_clicked()
{
    QString filePath = ui->savePath->text();
    if (filePath.isNull() || !QDir(QFileInfo(filePath).path()).exists())
        return;

    QString textFile = "";

    if (ui->gAddOldFileName->isChecked() && ui->gAddNewFileName->isChecked()) {
        ui->FLists->selectAll();
        QString separate = ui->separatorC->text().count() ? ui->separatorC->text() : "-";
        foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
            if (i.column() == 0)
                textFile.append(getOld(m_Model->item(i.row(), 0)) + " " + separate + " " + getNew(m_Model->item(i.row(), 0)) + "\n");
        }
    } else {
        if (ui->gAddNewFileName->isChecked()) {
            ui->FLists->selectAll();
            foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
                if (i.column() == 0) textFile.append(getNew(m_Model->item(i.row(), 0)) + "\n");
            }
        } else if (ui->gAddOldFileName->isChecked()) {
            ui->FLists->selectAll();
            foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
                if (i.column() == 0) textFile.append(getOld(m_Model->item(i.row(), 0)) + "\n");
            }
        } else {
            return;
        }
    }

    ui->FLists->clearSelection();

    QFile file(filePath);
    if (file.exists()) {
        long reply = QMessageBox::warning(this, "Warning", "File exists.\nDo you want to overwrite it.", QMessageBox::Yes, QMessageBox::No);
        if (reply == QMessageBox::No) return;
    }
    if (file.open(QIODevice::WriteOnly)) {
        file.write(textFile.toLatin1());
        file.close();
        // Function from utilities.cpp
        messageEngine("Renamed List Saved.", Info);
    }
}

void corerenamer::on_bUndo_clicked()
{
    uStack->undo();
}

void corerenamer::on_bRedo_clicked()
{
    uStack->redo();
}

void corerenamer::on_rename_clicked()
{
//    QString msg = QString("Are you sure to rename those files?\nIf you once rename them it is not possible to go back...");
//    QMessageBox message(QMessageBox::Question, "Permission", msg,QMessageBox::Yes, QMessageBox::No);
//    message.setWindowIcon(QIcon(":/app/icons/app-icons/CoreRenemer.svg"));
//    message.setStyleSheet(getStylesheetFileContent(":/appStyle/style/Dialog.qss"));
//    int merge = message.exec();

    long reply = QMessageBox::information(this, "Permission", "Are you sure to rename those files?\nIf you once rename them it is not possible to go back...",
                                          QMessageBox::Yes, QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->FLists->selectAll();
        foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
            if (i.column() == 0) {
                QStandardItem *item = m_Model->item(i.row(), 1);
                if (item->text().count()) {
                    // FIXME : IF FILE EXISTS THEN DO A ACTION
                    QFile::rename(item->data(Qt::UserRole).toString(), getNew(m_Model->item(i.row(), 0)));
                    item->setData(getNew(m_Model->item(i.row(), 0)), Qt::UserRole);
                }
            }
        }

        uStack->clear();
        ui->FLists->clearSelection();
        // Function from utilities.cpp
        messageEngine("File Renamed Successfully.", Info);
    }
}

void corerenamer::on_bRefreshList_clicked()
{
    if ( m_Model->rowCount() < 1 )
        return;

    uStack->clear();
    ui->FLists->selectAll();
    foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
        m_Model->item(i.row(), 1)->setText("");
    }
    ui->FLists->selectionModel()->clearSelection();
}

void corerenamer::on_clearItem_clicked()
{
    foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
        if (!i.column())
            m_Model->removeRow(i.row());
    }
}

void corerenamer::on_gAddOldFileName_clicked(bool checked)
{
    if (!checked) {
        if (!ui->gAddNewFileName->isChecked())
            ui->gAddNewFileName->setChecked(1);
    }
}

void corerenamer::on_gAddNewFileName_clicked(bool checked)
{
    if (!checked) {
        if (!ui->gAddOldFileName->isChecked())
            ui->gAddOldFileName->setChecked(1);
    }
}

bool customSortProxyM::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    QCollator cola;
    cola.setNumericMode(true);
    return (cola.compare(leftData.toString(), rightData.toString()) < 0);
}

void corerenamer::on_doRemoveAddNum_clicked()
{
    if (ui->tDoRemoveAddNum->text().count()) {
        ui->FLists->selectAll();
        int totalCount = ui->FLists->selectionModel()->selectedIndexes().count();
        int count = 0;
        foreach (QModelIndex i, ui->FLists->selectionModel()->selectedIndexes()) {
            if (i.column() == 0) {
                QString aText = ui->tDoRemoveAddNum->text();
                QString arg = "";
                count++;

                QString arg1;
                if (totalCount > 100 && totalCount < 999) arg1 = "001. ";
                else if (totalCount > 10 && totalCount < 99) arg1 = "01. ";
                else arg1 = "1. ";

                if (arg1.at(0) == '0') {
                    if (arg1.at(1) == '0') {
                        arg = QString(i.row() > 8 ? i.row() > 98 ? "" : "0" : "00") + QString::number(count) + QString(arg1).remove(0, 3);
                    } else {
                        arg = QString(i.row() > 8 ? "" : "0") + QString::number(count) + QString(arg1).remove(0, 2);
                    }
                } else arg = QString::number(count) + QString(arg1).remove(0, 1);

                QString numing = addText(FROMLEFT, 0, arg, aText);
                uStack->push(new ARRTextCommand(m_Model->item(i.row(), 1), numing, NUMBER));
            }
        }
    }
}
