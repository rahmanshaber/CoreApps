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

#include "corepad.h"
#include "ui_corepad.h"


corepad::corepad(QWidget *parent) : QWidget(parent), ui(new Ui::corepad)
{
    ui->setupUi(this);

    // set stylesheet from style.qrc
    setStyleSheet(Utilities::getStylesheetFileContent(":/appStyle/style/CorePad.qss"));

    // set window size
    int x = static_cast<int>(Utilities::screensize().width()  * .8);
    int y = static_cast<int>(Utilities::screensize().height()  * .7);
    this->resize(x, y);

    ui->searchBar->setVisible(false);
    ui->fontSize->setVisible(0);

    shotcuts();
}

corepad::~corepad()
{
    delete ui;
}

void corepad::shotcuts()
{
    QShortcut* shortcut;
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cNew_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cOpen_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cSave_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cSaveAs_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cUndo_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cRedo_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cCopy_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_X), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cCut_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_cPaste_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::quitClicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    connect(shortcut, &QShortcut::activated, this, &corepad::on_search_clicked);
}


void corepad::openText(const QString &filePath)
{
    initializeNewTab(filePath);
}

bool corepad::initializeNewTab(const QString &filePath)
{
    int tabsCount = ui->notes->count();
    if (tabsCount < 4) {
        text = new coreedit(filePath);
        QString tabLabel;
        bool isUpdate = 0, isSave = 0;

        if (filePath.isEmpty()) {
            workFilePath = "";
            tabLabel = tr("untitled%1.txt").arg(tabsCount);
        } else {
            QFile file(filePath);
            if (!file.open(QIODevice::Text | QIODevice::ReadOnly) && !file.exists())
                return false; //Return an exception - There is no file with that name.
                //return false; //Return an exception - It's not possible for me to open that file.
            else {
                workFilePath = filePath;
                tabLabel = QFileInfo(filePath).fileName();
                QTextStream in(&file);
                text->setPlainText(in.readAll());
                file.close();
                isSave = 1;
            }
        }

        tabInfo.append(tr("%1\t%2\t%3\t%4").arg(tabsCount).arg(isSave).arg(isUpdate).arg(workFilePath));

        started = true;

        text->setFont(QFont(text->font().family(), ui->fontSize->value()));
        text->lineNumberArea_()->setFont(QFont(text->lineNumberArea_()->font().family(), ui->fontSize->value()));

        ui->notes->insertTab(tabsCount, text, tabLabel);
        ui->notes->setCurrentIndex(tabsCount);

        connect(text, &coreedit::copyAvailable, this, &corepad::textCopyAvailable);
        connect(text, &coreedit::undoAvailable, this, &corepad::textUndoAvailable);
        connect(text, &coreedit::redoAvailable, this, &corepad::textRedoAvailable);
        connect(text, &coreedit::textChanged, this, &corepad::textTextChanged);
        return true;
    } else {
        // Function from utilities.cpp
        Utilities::messageEngine("Reached page limit.\nClose some tab.", Utilities::MessageType::Warning);
    }
    return false; //Return an exception - There is some other causes happening.
}

int corepad::tabsCount()
{
    return ui->notes->count();
}

bool corepad::saveTo(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::Truncate | QIODevice::Text | QIODevice::WriteOnly)) {
        return false;
    } else {
        file.write(text->toPlainText().toLocal8Bit());
        file.close();
        return true;
    }
}

void corepad::findS(QString searchS, bool reverse, QTextDocument::FindFlags flag)
{
    if (!searchS.isNull()) {
        QTextCursor cr;
        text->find(searchS, flag);
        cr.movePosition(reverse ? QTextCursor::End : QTextCursor::Start);
        if (!(cr.position() < 0)) {
            text->setTextCursor(cr);
        }
    }
}

bool corepad::closeTab(int index)
{
    ui->notes->setCurrentIndex(index);
    bool checkIsUpdate = isCurrentUpdated(index).toInt();
    bool checkIsSaved = isCurrentSaved(index).toInt();

    if ((checkIsUpdate && checkIsSaved) || (checkIsUpdate && !checkIsSaved)) {

        QString msg = QString("This file contains unsaved changes.\nHow would you like to proceed?");
        QMessageBox message(QMessageBox::Question, QString("Save Changes - \"%1\"").arg(currentFilePath(index)), msg,
                                          QMessageBox::Cancel | QMessageBox::Discard | QMessageBox::Save, this);
        message.setWindowIcon(QIcon(":/app/icons/app-icons/CoreFM.svg"));
        message.setStyleSheet(Utilities::getStylesheetFileContent(":/appStyle/style/Dialog.qss"));

        int reply = message.exec();

        if (reply == QMessageBox::Save) {
            bool closed = on_cSave_clicked();
            if (closed) {
                // Function from utilities.cpp
                Utilities::saveToRecent("CorePad", currentFilePath(index));
                goto CLOSE_THE_TAB;
            } else {
                return false;
            }
        } else if (reply == QMessageBox::Discard) {
            if (checkIsSaved)
                // Function from utilities.cpp
                Utilities::saveToRecent("CorePad", currentFilePath(index));
            goto CLOSE_THE_TAB;
        } else {
            return false;
        }
    } else if ((!checkIsUpdate && checkIsSaved) || (!checkIsUpdate && !checkIsSaved)) {
        // Function from utilities.cpp
        Utilities::saveToRecent("CorePad", currentFilePath(index));
        CLOSE_THE_TAB:
        ui->notes->widget(index)->deleteLater();
        ui->notes->removeTab(index);
        tabInfo.removeAt(index);
        return true;
    }

    return false;
}

//Accessing information through tab information
QString corepad::currentFilePath(int index)
{
    for (int i = 0; i < tabInfo.count(); i++) {
        if (tabInfo.at(i).startsWith(QString::number(index)))
            return tabInfo.at(i).split("\t").at(3);
    }
    return "";
}

QString corepad::isCurrentSaved(int index)
{
    for (int i = 0; i < tabInfo.count(); i++) {
        if (tabInfo.at(i).startsWith(QString::number(index)))
            return tabInfo.at(i).split("\t").at(1);
    }
    return "";
}

QString corepad::isCurrentUpdated(int index)
{
    for (int i = 0; i < tabInfo.count(); i++) {
        if (tabInfo.at(i).startsWith(QString::number(index)))
            return tabInfo.at(i).split("\t").at(2);
    }
    return "";
}
//=============================================

//Saving information at tab information
bool corepad::setCurrent(int index, int isSaved, int isUpdated, const QString &filePath)
{
    tabInfo.removeAt(index);
    tabInfo.insert(index, QString("%1\t%2\t%3\t%4").arg(index).arg(isSaved).arg(isUpdated).arg(filePath));
    return true;
}
//===============================================

void corepad::reIndex() {
    int count = tabInfo.count();
    for (int i = 0; i < count; i++) {
        tabInfo.replace(i, QString(tabInfo.at(i)).replace(0, 1, QString::number(i)));
    }
}

void corepad::textCopyAvailable(bool b)
{
    ui->cCopy->setEnabled(b);
    ui->cCut->setEnabled(b);
}

void corepad::textUndoAvailable(bool b)
{
    ui->cUndo->setEnabled(b);
}

void corepad::textRedoAvailable(bool b)
{
    ui->cRedo->setEnabled(b);
}

void corepad::textTextChanged()
{
    if (started) {
        started = false;
        return;
    }
    int index = ui->notes->currentIndex();
    if (text->toPlainText().count() > 0) {
        if (isCurrentUpdated(index) == "0" && !ui->notes->tabText(index).startsWith("*")) {
            ui->notes->setTabText(ui->notes->currentIndex(), "*" + ui->notes->tabText(ui->notes->currentIndex()));
            setCurrent(index, isCurrentSaved(index).toInt(), 1, currentFilePath(index));
        }
    }
}

void corepad::on_notes_currentChanged(int index)
{
    if(ui->notes->count() > 0){
        text = static_cast<coreedit*>(ui->notes->widget(index));
        started = 1;
        //FIX ME:
        /*Copy available need to fix when the current tab changed*/
        textRedoAvailable(text->isUndoRedoEnabled());
        textUndoAvailable(text->isUndoRedoEnabled());
        textTextChanged();
        on_searchHere_textChanged(ui->searchHere->text());
        //
    }
}

void corepad::on_notes_tabCloseRequested(int index)
{
    ui->notes->setCurrentIndex(index);
    reIndex();
    closeTab(index);
    reIndex();
}

void corepad::on_cOpen_clicked()
{
    workFilePath = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath() + "/Documents", tr("Text Files (*.*)"));
    if (workFilePath.isEmpty()) {
        // Function from utilities.cpp
        Utilities::messageEngine("Empty File Name", Utilities::MessageType::Warning);
        return;
    }
    else {
        bool open = initializeNewTab(workFilePath);
        if (open) Utilities::messageEngine("File Opened", Utilities::MessageType::Info);
        else Utilities::messageEngine("Can't open file", Utilities::MessageType::Warning);
    }
}

void corepad::on_cNew_clicked()
{
    initializeNewTab("");
}

bool corepad::on_cSave_clicked()
{
    workFilePath = currentFilePath(ui->notes->currentIndex());
    int index = ui->notes->currentIndex();
    bool saved = isCurrentSaved(index).toInt(), updated = isCurrentUpdated(index).toInt();
    if (!saved && updated) {
        workFilePath = QFileDialog::getSaveFileName(this, tr("Save File"), ui->notes->tabText(ui->notes->currentIndex()).remove(0, 1) ,tr("Text Files (*.*)"));
        if (workFilePath.isEmpty()) {
            // Function from utilities.cpp
            Utilities::messageEngine("Empty File Name", Utilities::MessageType::Warning);
            return false;
        } else {
            goto SAVE_FILE;
        }
    }
    else if (saved && updated) {
        SAVE_FILE:
        bool saveto = saveTo(workFilePath);
        if (saveto) {
            setCurrent(index, 1, 0, workFilePath);
            ui->notes->setTabText(index, QFileInfo(workFilePath).fileName());
            // Function from utilities.cpp
            Utilities::messageEngine("File Saved", Utilities::MessageType::Info);
            return true;
        } else {
            // Function from utilities.cpp
            Utilities::messageEngine("Can't open file", Utilities::MessageType::Warning);
        }
    }
    return false;
}

void corepad::on_cSaveAs_clicked()
{
    workFilePath = currentFilePath(ui->notes->currentIndex());
    QString file = "untitled.txt";
    if (!workFilePath.isNull()) {
        file = workFilePath;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"),
                                                      file,
                                                      tr("Text Files (*.*)"));
    int index = ui->notes->currentIndex();
    if (fileName.isEmpty()) {
        // Function from utilities.cpp
        Utilities::messageEngine("Empty File Name", Utilities::MessageType::Warning);
        return;
    }
    else {
        bool saveto = saveTo(workFilePath);
        if (saveto) {
            ui->notes->setTabText(index, QFileInfo(fileName).fileName());
            setCurrent(index, 1, 0, fileName);
            // Function from utilities.cpp
            Utilities::messageEngine("File Saved", Utilities::MessageType::Info);
        } else {
            // Function from utilities.cpp
            Utilities::messageEngine("Can't open file", Utilities::MessageType::Warning);
            return;
        }
    }
    workFilePath = fileName;
}

void corepad::on_cCopy_clicked()
{
    text->copy();
}

void corepad::on_cPaste_clicked()
{
    text->paste();
}

void corepad::on_cCut_clicked()
{
    text->cut();
}

void corepad::on_cUndo_clicked()
{
    text->undo();
}

void corepad::on_cRedo_clicked()
{
    text->redo();
}

void corepad::on_addDate_clicked()
{
    int pos = text->textCursor().position();
    text->setPlainText(text->toPlainText().insert(text->textCursor().position(), QDate::currentDate().toString("dd/MM/yyyy")));
    text->textCursor().setPosition(pos + 10);

}

void corepad::on_bookMarkIt_clicked()
{
    if (!currentFilePath(ui->notes->currentIndex()).isNull()) {
        if (!QFileInfo(workFilePath).exists()) {
            // Function from utilities.cpp
            Utilities::messageEngine("File Not saved.", Utilities::MessageType::Warning);
            return;
        }
        GlobalFunc::appEngines("BookMarkIt",workFilePath);
    }
}

void corepad::on_searchHere_textChanged(const QString &arg1)
{
    findS(arg1, false);
}

void corepad::on_nextW_clicked()
{
    findS(ui->searchHere->text(), false);
}

void corepad::on_previousW_clicked()
{
    findS(ui->searchHere->text(), true, QTextDocument::FindBackward);
}

void corepad::quitClicked()
{
    int tabCount = ui->notes->count();
    int cc = 0;
    for (int i = 0; i < tabCount; i++) {
        if (closeTab(i - cc)) {
            reIndex();
            cc++;
        }
    }

    if (ui->notes->count() == 0) {
        if (this->close()) {
            deleteLater();
        }
    }
    else Utilities::messageEngine("There are still some changes needs to be saved.", Utilities::MessageType::Warning);
}

void corepad::closeEvent(QCloseEvent *event)
{
    event->ignore();
    int tabCount = ui->notes->count();
    int cc = 0;
    for (int i = 0; i < tabCount; i++) {
        if (closeTab(i - cc)) {
            reIndex();
            cc++;
        }
    }

    if (ui->notes->count() == 0) {
        event->accept();
    }
    else event->ignore();
}


void corepad::on_fontShow_clicked()
{
    if(!ui->fontSize->isVisible()){
        ui->fontSize->setVisible(1);
    }else{
        ui->fontSize->setVisible(0);
    }
}

void corepad::on_fontSize_valueChanged(int arg1)
{
    text->setFont(QFont(text->font().family(), arg1));
    text->lineNumberArea_()->setFont(QFont(text->lineNumberArea_()->font().family(), arg1));
}

void corepad::on_search_clicked()
{
    if (ui->searchBar->isVisible()) {
        ui->searchBar->setVisible(false);
        ui->searchHere->clear();
    } else {
        ui->searchBar->setVisible(true);
        ui->searchHere->setFocus();
    }
}

void corepad::sendFiles(const QStringList &paths) {
    qDebug() << paths;
    if (paths.count()) {
        foreach (QString str, paths) {
            openText(Utilities::checkIsValidFile(str));
        }
    }
}
