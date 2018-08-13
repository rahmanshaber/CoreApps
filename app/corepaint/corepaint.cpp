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

#include "corepaint.h"
#include "ui_corepaint.h"


corepaint::corepaint( QWidget *parent):QWidget(parent),ui(new Ui::corepaint),
    mPrevInstrumentSetted(false)
{
    ui->setupUi(this);

    // set stylesheet from style.qrc
    setStyleSheet(getStylesheetFileContent(":/appStyle/style/CorePaint.qss"));

    // set window size
    int x = static_cast<int>(screensize().width()  * .8);
    int y = static_cast<int>(screensize().height()  * .7);
    this->resize(x, y);
    mUndoStackGroup = new QUndoGroup(this);

    qRegisterMetaType<InstrumentsEnum>("InstrumentsEnum");
    DataSingleton::Instance()->setIsInitialized();

    if(filepath.isEmpty()){
        ui->save->setEnabled(false);
        ui->saveas->setEnabled(false);
        ui->bookMarkIt->setEnabled(false);
        ui->canvasB->setEnabled(false);
        ui->selectionB->setEnabled(false);
        ui->toolsB->setEnabled(false);
        ui->colorB->setEnabled(false);
    }

    loadSettings();
    initializeMainMenu();
    shotcuts();
}

corepaint::~corepaint()
{
    delete ui;
}

void corepaint::loadSettings()
{
    saveLocation = sm.getSCSaveLocation();
}

void corepaint::shotcuts()
{
    QShortcut* shortcut;
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_open_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_save_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_saveas_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(undo()));
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(redo()));
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_copy_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_X), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_cut_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_past_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_newtab_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_B), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_bookMarkIt_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut, &QShortcut::activated, this, &corepaint::on_delet_clicked);
}

void corepaint::initializeNewTab(const bool &isOpen, const QString &filePath)
{
    if (ui->paintTabs->count() < 4) {
        ImageArea *imageArea;
        QString fileName;

        if(isOpen && filePath.isEmpty()) {
            imageArea = new ImageArea(isOpen, "", this);
            fileName = imageArea->getFileName();
        } else if(isOpen && !filePath.isEmpty()) {
            imageArea = new ImageArea(isOpen, filePath, this);
            fileName = imageArea->getFileName();
        } else {
            imageArea = new ImageArea(false, "", this);
            fileName = (tr("UntitledImage_") + QString::number(ui->paintTabs->count()));
        }

        if (!imageArea->getFileName().isNull()) {
            QScrollArea *scrollArea = new QScrollArea();
            scrollArea->setAttribute(Qt::WA_DeleteOnClose);
            //color for core paint
            QString color(getStylesheetValue()->value("@color06").toString());
            scrollArea->setStyleSheet("QScrollArea { background-color: " + color + ";border-style: none; }");

            //scrollArea->setBackgroundRole(QPalette::Shadow);

            scrollArea->setWidget(imageArea);

            ui->paintTabs->addTab(scrollArea,fileName);
            ui->paintTabs->setCurrentIndex(ui->paintTabs->count()-1);

            mUndoStackGroup->addStack(imageArea->getUndoStack());
            connect(imageArea, SIGNAL(sendPrimaryColorView()), this, SLOT(setPrimaryColorView()));
            connect(imageArea, SIGNAL(sendSecondaryColorView()), this, SLOT(setSecondaryColorView()));
            connect(imageArea, SIGNAL(sendRestorePreviousInstrument()), this, SLOT(restorePreviousInstrument()));
            connect(imageArea, SIGNAL(sendSetInstrument(InstrumentsEnum)), this, SLOT(setInstrument(InstrumentsEnum)));
            connect(imageArea, SIGNAL(sendNewImageSize(QSize)), this, SLOT(setNewSizeToSizeLabel(QSize)));
            connect(imageArea, SIGNAL(sendCursorPos(QPoint)), this, SLOT(setNewPosToPosLabel(QPoint)));
            connect(imageArea, SIGNAL(sendColor(QColor)), this, SLOT(setCurrentPipetteColor(QColor)));
            connect(imageArea, SIGNAL(sendEnableCopyCutActions(bool)), this, SLOT(enableCopyCutActions(bool)));
            connect(imageArea, SIGNAL(sendEnableSelectionInstrument(bool)), this, SLOT(instumentsAct(bool)));

            //setWindowTitle(QString("%1 - EasyPaint").arg(fileName));
        } else {
            delete imageArea;
        }

        currentFile = fileName;
        filepath = filePath;
        if (ui->paintTabs->count() >= 1) {
            ui->save->setEnabled(true);
            ui->saveas->setEnabled(true);
            ui->bookMarkIt->setEnabled(true);
            ui->canvasB->setEnabled(true);
            ui->selectionB->setEnabled(true);
            ui->toolsB->setEnabled(true);
            ui->colorB->setEnabled(true);
        }

        if (!fileName.isEmpty()) {
            // Function from globalfunctions.cpp
            messageEngine("File Opened Successfully.", MessageType::Info);
        } else {
            // Function from globalfunctions.cpp
            messageEngine("File not Opened Successfully.", MessageType::Info);
        }
    }
    else {
        // Function from globalfunctions.cpp
        messageEngine("Reached page limit.", MessageType::Warning);
    }
}

int corepaint::tabsCount()
{
    return ui->paintTabs->count();
}

void corepaint::initializeMainMenu()
{
    ui->actioniundo = mUndoStackGroup->createUndoAction(this);
    ui->undo->setDefaultAction(ui->actioniundo);
    ui->actioniredo = mUndoStackGroup->createRedoAction(this);
    ui->redo->setDefaultAction(ui->actioniredo);

    connect(ui->actionSelect, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(CURSOR, ui->actionSelect);
    ui->select->setDefaultAction(ui->actionSelect);

    connect(ui->actionEraser, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(ERASER, ui->actionEraser);
    ui->eraser->setDefaultAction(ui->actionEraser);

    connect(ui->actionColorpicker, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(COLORPICKER, ui->actionColorpicker);
    ui->colorpicker->setDefaultAction(ui->actionColorpicker);

    connect(ui->actionPen, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(PEN, ui->actionPen);
    ui->pen->setDefaultAction(ui->actionPen);

    connect(ui->actionLine, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(LINE, ui->actionLine);
    ui->line->setDefaultAction(ui->actionLine);

    connect(ui->actionSpray, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(SPRAY, ui->actionSpray);
    ui->spray->setDefaultAction(ui->actionSpray);

    connect(ui->actionFill, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(FILL, ui->actionFill);
    ui->fill->setDefaultAction(ui->actionFill);

    connect(ui->actionRectangle, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(RECTANGLE, ui->actionRectangle);
    ui->rectangle->setDefaultAction(ui->actionRectangle);

    connect(ui->actionEllipse, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(ELLIPSE, ui->actionEllipse);
    ui->ellipse->setDefaultAction(ui->actionEllipse);

    connect(ui->actionCurve, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(CURVELINE, ui->actionCurve);
    ui->curve->setDefaultAction(ui->actionCurve);

    connect(ui->actionText, SIGNAL(triggered(bool)), this, SLOT(instumentsAct(bool)));
    mInstrumentsActMap.insert(TEXT, ui->actionText);
    ui->text->setDefaultAction(ui->actionText);


    mPColorChooser = new ColorChooser(0, 0, 0, this);
    mPColorChooser->setStatusTip(tr("Primary color"));
    mPColorChooser->setToolTip(tr("Primary color"));
    connect(mPColorChooser, SIGNAL(sendColor(QColor)), this, SLOT(primaryColorChanged(QColor)));

    mSColorChooser = new ColorChooser(255, 255, 255, this);
    mSColorChooser->setStatusTip(tr("Secondary color"));
    mSColorChooser->setToolTip(tr("Secondary color"));
    connect(mSColorChooser, SIGNAL(sendColor(QColor)), this, SLOT(secondaryColorChanged(QColor)));

    ui->cc->addWidget(mPColorChooser);
    ui->cc->addWidget(mSColorChooser);

    connect(ui->toolSize, SIGNAL(valueChanged(int)), this, SLOT(penValueChanged(int)));
}

void corepaint::penValueChanged(const int &value)
{
    DataSingleton::Instance()->setPenSize(value);
}

void corepaint::primaryColorChanged(const QColor &color)
{
    DataSingleton::Instance()->setPrimaryColor(color);
}

void corepaint::secondaryColorChanged(const QColor &color)
{
    DataSingleton::Instance()->setSecondaryColor(color);
}

void corepaint::setPrimaryColorView()
{
    mPColorChooser->setColor(DataSingleton::Instance()->getPrimaryColor());
}

void corepaint::setSecondaryColorView()
{
    mSColorChooser->setColor(DataSingleton::Instance()->getSecondaryColor());
}

ImageArea* corepaint::getCurrentImageArea()
{
    if (ui->paintTabs->currentWidget()){
        QScrollArea *tempScrollArea = qobject_cast<QScrollArea*>(ui->paintTabs->currentWidget());
        ImageArea *tempArea = qobject_cast<ImageArea*>(tempScrollArea->widget());
        return tempArea;
    }
    return nullptr;
}

ImageArea* corepaint::getImageAreaByIndex(int index)
{
    QScrollArea *sa = static_cast<QScrollArea*>(ui->paintTabs->widget(index));
    ImageArea *ia = static_cast<ImageArea*>(sa->widget());
    return ia;
}

void corepaint::setNewSizeToSizeLabel(const QSize &size)
{
    ui->mSizeLabel->setText(QString("%1 x %2").arg(size.width()).arg(size.height()));
}

void corepaint::setNewPosToPosLabel(const QPoint &pos)
{
    ui->mPosLabel->setText(QString("%1,%2").arg(pos.x()).arg(pos.y()));
}

void corepaint::setCurrentPipetteColor(const QColor &color)
{
    ui->pColorRGBLabel->setText(QString("RGB: %1,%2,%3").arg(color.red())
                         .arg(color.green()).arg(color.blue()));

    QPixmap statusColorPixmap = QPixmap(18, 18);
    QPainter statusColorPainter;
    statusColorPainter.begin(&statusColorPixmap);
    statusColorPainter.fillRect(0, 0, 19, 19, color);
    statusColorPainter.end();
    ui->pColorPreviewLabel->setPixmap(statusColorPixmap);
}

void corepaint::clearStatusBarColor()
{
    ui->pColorPreviewLabel->clear();
    ui->pColorRGBLabel->clear();
}

void corepaint::closeEvent(QCloseEvent *event)
{
    if(!isSomethingModified() || closeAllTabs())
    {
        event->ignore();
        // Function from globalfunctions.cpp
        saveToRecent("CorePaint", currentFile);
        event->accept();
    }
    else
        event->ignore();
}

bool corepaint::isSomethingModified()
{
    for(int i = 0;i < ui->paintTabs->count(); ++i){
        if(getImageAreaByIndex(i)->getEdited())
            return true;
    }
    return false;
}

bool corepaint::closeAllTabs()
{
    while(ui->paintTabs->count() != 0)
    {
        ImageArea *ia = getImageAreaByIndex(0);
        if(ia->getEdited())
        {
            int ans = QMessageBox::warning(this, tr("Closing Tab..."),
                                           tr("File has been modified.\nDo you want to save changes?"),
                                           QMessageBox::Yes | QMessageBox::Default,
                                           QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape);
            switch(ans)
            {
            case QMessageBox::Yes:
                if (ia->save())
                    break;
                return false;
            case QMessageBox::Cancel:
                return false;
            }
        }
        QWidget *wid = ui->paintTabs->widget(0);
        ui->paintTabs->removeTab(0);
        delete wid;
    }
    return true;
}

void corepaint::setAllInstrumentsUnchecked(QAction *action)
{
    clearImageSelection();
    foreach (QAction *temp, mInstrumentsActMap)
    {
        if(temp != action)
            temp->setChecked(false);
    }
}

void corepaint::setInstrumentChecked(InstrumentsEnum instrument)
{
    setAllInstrumentsUnchecked(nullptr);
    if(instrument == NONE_INSTRUMENT || instrument == INSTRUMENTS_COUNT)
        return;
    mInstrumentsActMap[instrument]->setChecked(true);
}

void corepaint::instumentsAct(bool state)
{
    QAction *currentAction = static_cast<QAction*>(sender());
    if(state)
    {
        if(currentAction == mInstrumentsActMap[COLORPICKER] && !mPrevInstrumentSetted)
        {
            DataSingleton::Instance()->setPreviousInstrument(DataSingleton::Instance()->getInstrument());
            mPrevInstrumentSetted = true;
        }
        setAllInstrumentsUnchecked(currentAction);
        currentAction->setChecked(true);
        DataSingleton::Instance()->setInstrument(mInstrumentsActMap.key(currentAction));
        emit sendInstrumentChecked(mInstrumentsActMap.key(currentAction));
    }
    else
    {
        setAllInstrumentsUnchecked(nullptr);
        DataSingleton::Instance()->setInstrument(NONE_INSTRUMENT);
        emit sendInstrumentChecked(NONE_INSTRUMENT);
        if(currentAction == mInstrumentsActMap[CURSOR])
            DataSingleton::Instance()->setPreviousInstrument(mInstrumentsActMap.key(currentAction));
    }
}

void corepaint::enableCopyCutActions(bool enable)
{
    ui->copy->setEnabled(enable);
    ui->cut->setEnabled(enable);
}

void corepaint::clearImageSelection()
{
    if (getCurrentImageArea())
    {
        getCurrentImageArea()->clearSelection();
        DataSingleton::Instance()->setPreviousInstrument(NONE_INSTRUMENT);
    }
}

void corepaint::restorePreviousInstrument()
{
    setInstrumentChecked(DataSingleton::Instance()->getPreviousInstrument());
    DataSingleton::Instance()->setInstrument(DataSingleton::Instance()->getPreviousInstrument());
    emit sendInstrumentChecked(DataSingleton::Instance()->getPreviousInstrument());
    mPrevInstrumentSetted = false;
}

void corepaint::setInstrument(InstrumentsEnum instrument)
{
    setInstrumentChecked(instrument);
    DataSingleton::Instance()->setInstrument(instrument);
    emit sendInstrumentChecked(instrument);
    mPrevInstrumentSetted = false;
}

void corepaint::on_paintTabs_currentChanged(int index)
{
    //activateTab
    if(index == -1)
        return;
    ui->paintTabs->setCurrentIndex(index);
    getCurrentImageArea()->clearSelection();

    QSize size = getCurrentImageArea()->getImage()->size();
    ui->mSizeLabel->setText(QString("%1 x %2").arg(size.width()).arg(size.height()));

    if(!getCurrentImageArea()->getFileName().isEmpty())
    {
        currentFile = QDir(currentFile).path() + "/" + getCurrentImageArea()->getFileName();
    }
    else
    {
        currentFile = "Untitled Image";
    }
    mUndoStackGroup->setActiveStack(getCurrentImageArea()->getUndoStack());

    //enableActions
    //if index == -1 it means, that there is no tabs
    bool isEnable = index == -1 ? false : true;

    if(!isEnable)
    {
        setAllInstrumentsUnchecked(nullptr);
        DataSingleton::Instance()->setInstrument(NONE_INSTRUMENT);
        emit sendInstrumentChecked(NONE_INSTRUMENT);
    }

    ui->workingOn->setText(ui->paintTabs->tabText(index));

}

void corepaint::on_paintTabs_tabCloseRequested(int index)
{
    ImageArea *ia = getImageAreaByIndex(index);
    if(ia->getEdited())
    {
        int ans = QMessageBox::warning(this, tr("Closing Tab..."),
                                       tr("File has been modified.\nDo you want to save changes?"),
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape);
        switch(ans)
        {
        case QMessageBox::Yes:
            if (ia->save()) break;
            return;
        case QMessageBox::Cancel:
            return;
        }
    }
    mUndoStackGroup->removeStack(ia->getUndoStack()); //for safety
    QWidget *wid = ui->paintTabs->widget(index);
    ui->paintTabs->removeTab(index);
    delete wid;
    if(ui->paintTabs->count() == 0){
        ui->save->setEnabled(false);
        ui->saveas->setEnabled(false);
        ui->bookMarkIt->setEnabled(false);
        ui->canvasB->setEnabled(false);
        ui->selectionB->setEnabled(false);
        ui->toolsB->setEnabled(false);
        ui->colorB->setEnabled(false);
        ui->selectedsection->setText("");
    }
}

void corepaint::on_newtab_clicked()
{
    initializeNewTab();
}

void corepaint::on_open_clicked()
{
    initializeNewTab(true);
}

void corepaint::on_save_clicked()
{
    if(getCurrentImageArea()->save()){
        ui->paintTabs->setTabText(ui->paintTabs->currentIndex(), getCurrentImageArea()->getFileName().isEmpty() ?
                                      tr("Untitled Image") : getCurrentImageArea()->getFileName() );

        filepath = getCurrentImageArea()->mFilePath;
        // Function from globalfunctions.cpp
        messageEngine("File Saved", MessageType::Info);
    } else {
        // Function from globalfunctions.cpp
        messageEngine("File not Saved", MessageType::Info);
    }
}

void corepaint::on_saveas_clicked()
{
    if(getCurrentImageArea()->saveAs()){
        ui->paintTabs->setTabText(ui->paintTabs->currentIndex(), getCurrentImageArea()->getFileName().isEmpty() ?
                                      tr("Untitled Image") : getCurrentImageArea()->getFileName() );

        filepath = getCurrentImageArea()->mFilePath;
        // Function from globalfunctions.cpp
        messageEngine("File Saved", MessageType::Info);
    } else {
        // Function from globalfunctions.cpp
        messageEngine("File not Saved", MessageType::Info);
    }
}

void corepaint::on_resizeimage_clicked()
{
    getCurrentImageArea()->resizeImage();
}

void corepaint::on_resizecanvas_clicked()
{
    getCurrentImageArea()->resizeCanvas();
}

void corepaint::on_rotateright_clicked()
{
    getCurrentImageArea()->rotateImage(true);
}

void corepaint::on_rotateleft_clicked()
{
    getCurrentImageArea()->rotateImage(false);
}

void corepaint::on_zoomin_clicked()
{
    getCurrentImageArea()->zoomImage(2.0);
    getCurrentImageArea()->setZoomFactor(2.0);
}

void corepaint::on_zoomout_clicked()
{
    getCurrentImageArea()->zoomImage(0.5);
    getCurrentImageArea()->setZoomFactor(0.5);
}

void corepaint::on_bookMarkIt_clicked()
{
    if (!QFile(filepath).exists()) {
        // Function from globalfunctions.cpp
        QString mess = "File: " + currentFile + "' not exists Or not saved";
        messageEngine(mess, MessageType::Info);
    } else {
        bookmarks bookMarks;
        bookMarks.callBookMarkDialog(this, filepath);
    }
}

void corepaint::on_cut_clicked()
{
    if (ImageArea *imageArea = getCurrentImageArea())
        imageArea->cutImage();
}

void corepaint::on_copy_clicked()
{
    if (ImageArea *imageArea = getCurrentImageArea())
        imageArea->copyImage();
}

void corepaint::on_past_clicked()
{
    if (ImageArea *imageArea = getCurrentImageArea())
        imageArea->pasteImage();
}

void corepaint::undo() {
    mUndoStackGroup->undo();
}

void corepaint::redo() {
    mUndoStackGroup->redo();
}

void corepaint::on_delet_clicked()
{
    if (ImageArea *imageArea = getCurrentImageArea())
       imageArea->clearBackground();
}

void corepaint::pageClick(QToolButton *btn, int i)
{
    // all button checked false
    for (QToolButton *b : ui->statusBar->findChildren<QToolButton*>())
        b->setChecked(false);
    btn->setChecked(true);
    ui->pages->setCurrentIndex(i);
}

void corepaint::on_menuB_clicked()
{
    pageClick(ui->menuB,0);
}

void corepaint::on_canvasB_clicked()
{
    pageClick(ui->canvasB,1);
}

void corepaint::on_selectionB_clicked()
{
    pageClick(ui->selectionB,2);
}

void corepaint::on_toolsB_clicked()
{
    pageClick(ui->toolsB,3);
}

void corepaint::on_colorB_clicked()
{
    pageClick(ui->colorB,4);
}
