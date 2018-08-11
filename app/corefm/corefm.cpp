﻿/*
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

#include "corefm.h"
#include "ui_corefm.h"


corefm::corefm(QWidget *parent) :QWidget(parent),ui(new Ui::corefm)
{
    ui->setupUi(this);

    // set stylesheet from style.qrc
    setStyleSheet(getStylesheetFileContent(":/appStyle/style/CoreFM.qss"));

    // set window size
    int x = screensize().width()  * .8;
    int y = screensize().height() * .7;
    this->resize(x, y);

    startsetup();
    loadSettings();
    lateStart();
    shotcuts();
}

corefm::~corefm()
{
    delete ui;
}

void corefm::startsetup()
{
    // create mime utils
    mimeUtils = new MimeUtils(this);
    QString name = "/.config/coreBox/mimeapps.list";
    mimeUtils->setDefaultsFileName(name);

    // setup startup path
    if (sm.getStartupPath() == "") {
        startPath = QDir::homePath();
    } else {
        startPath = sm.getStartupPath();
    }

    // create filesystem model
    bool realMime = sm.getIsRealMimeType();
    modelList = new myModel(realMime);

    // create the page tabs
    tabs = new tabBar(modelList->folderIcons);
    tabs->setDrawBase(0);
    tabs->setExpanding(0);
    tabs->setTabsClosable(true);
    tabs->setAutoHide(true);
    ui->pagesBar->addWidget(tabs);
    connect(tabs, SIGNAL(tabCloseRequested(int)), tabs, SLOT(closeTab(int)));

    modelTree = new mainTreeFilterProxyModel();
    modelTree->setSourceModel(modelList);
    modelTree->setSortCaseSensitivity(Qt::CaseInsensitive);

    // setup shortcut folder view
    ui->viewDir->setHeaderHidden(true);
    ui->viewDir->setUniformRowHeights(true);
    ui->viewDir->setModel(modelTree);
    ui->viewDir->hideColumn(1);
    ui->viewDir->hideColumn(2);
    ui->viewDir->hideColumn(3);
    ui->viewDir->hideColumn(4);
    ui->view->sizePolicy().setHorizontalStretch(1);
    ui->viewDir->sizePolicy().setHorizontalStretch(2);
    ui->viewDir->setVisible(false);
    connect(ui->viewTree, SIGNAL(clicked(bool)), ui->viewDir, SLOT(setVisible(bool)));
    qDebug()<< ui->splitter->size();


    modelView = new viewsSortProxyModel();
    modelView->setSourceModel(modelList);
    modelView->setSortCaseSensitivity(Qt::CaseInsensitive);

    // setup icon view
    ui->viewIcon->setModel(modelView);
    ui->viewIcon->setWordWrap(true);
    ui->viewIcon->setUniformItemSizes(true);
    ui->viewIcon->setFlow(QListView::LeftToRight);
    ui->viewIcon->setFocusPolicy(Qt::NoFocus);
    ui->viewIcon->setDragDropMode(QAbstractItemView::DragDrop);
    ui->viewIcon->setDefaultDropAction(Qt::MoveAction);
    listSelectionModel = ui->viewIcon->selectionModel();
    ui->viewIcon->setContextMenuPolicy(Qt::CustomContextMenu);

    // setup list view
    ui->viewDetail->setModel(modelView);
    ui->viewDetail->setSelectionModel(listSelectionModel);
    ui->viewDetail->setFocusPolicy(Qt::NoFocus);
    ui->viewDetail->setSortingEnabled(1);
    int i = ui->navigationBar->sizeHint().width();
    ui->viewDetail->setColumnWidth(0,i);
    ui->viewDetail->setContextMenuPolicy(Qt::CustomContextMenu);

    treeSelectionModel = ui->viewDir->selectionModel();
    connect(treeSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(treeSelectionChanged(QModelIndex,QModelIndex)));
    ui->viewDir->setCurrentIndex(modelTree->mapFromSource(modelList->index(startPath)));
    ui->viewDir->scrollTo(ui->viewDir->currentIndex());

    // setup buttons
    ui->paste->setVisible(0);
    ui->emptyTrash->setVisible(0);
    ui->actionAscending->setChecked(1);

    // setup all the tool buttons to related actions
    ui->newPage->setDefaultAction(ui->actionNewPage);
    ui->copy->setDefaultAction(ui->actionCopy);
    ui->cut->setDefaultAction(ui->actionCut);
    ui->properties->setDefaultAction(ui->actionProperties);
    ui->deleteit->setDefaultAction(ui->actionDelete);
    ui->up->setDefaultAction(ui->actionUp);
    ui->back->setDefaultAction(ui->actionBack);
    ui->terminal->setDefaultAction(ui->actionTerminal);
    ui->newfolder->setDefaultAction(ui->actionNewFolder);
    ui->newtext->setDefaultAction(ui->actionNewTextFile);

    // setup system partition list
    udisks = new UDisks2(this);
    connect(udisks, SIGNAL(blockDeviceAdded(QString)), this, SLOT(blockDevicesChanged()));
    connect(udisks, SIGNAL(blockDeviceChanged(QString)), this, SLOT(blockDevicesChanged()));
    connect(udisks, SIGNAL(blockDeviceRemoved(QString)), this, SLOT(blockDevicesChanged()));
    connect(udisks, SIGNAL(filesystemAdded(QString)), this, SLOT(blockDevicesChanged()));
    blockDevicesChanged();

    // reload files if there is some changes
    watcher = new QFileSystemWatcher(this);
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(on_actionRefresh_triggered()));

    // setup left mouse click at both views
    connect(ui->viewIcon, &ClickOutListview::clickedOut, this, &corefm::pressed);
    connect(ui->viewDetail, &ClickOutTreeview::clickedOut, this, &corefm::pressed);

    // set all int values to zero
    selectItemCount = 0;
}

void corefm::shotcuts()
{
    QShortcut* shortcut;

    shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionDelete_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::Key_T), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionTerminal_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::Key_Backspace), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionUp_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionSelectAll_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_searchHere_clicked);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionTrash_it_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_X), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionCut_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionCopy_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionPaste_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL +Qt::Key_Shift + Qt::Key_T), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionNewTextFile_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL +Qt::Key_Shift + Qt::Key_N), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionNewFolder_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionNewPage_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::Key_R), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_actionRefresh_triggered);
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_B), this);
    connect(shortcut, &QShortcut::activated, this, &corefm::on_SBookMarkIt_clicked);
}

void corefm::lateStart()
{
    // Watch for mounts
    int fd = open("/proc/self/mounts", O_RDONLY, 0);
    notify = new QSocketNotifier(fd, QSocketNotifier::Write);

    // Clipboard configuration
    progress = 0;
    clipboardChanged();

    // Completer configuration
    customComplete = new myCompleter;
    customComplete->setModel(modelTree);
    customComplete->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    customComplete->setMaxVisibleItems(10);
    ui->pathEdit->setCompleter(customComplete);

    // Connect mouse clicks in views
//    if (sett->value("singleClick").toInt() == 1) {
//      connect(ui->viewIcon, SIGNAL(clicked(QModelIndex)),this, SLOT(lll()));
//      connect(ui->viewtree, SIGNAL(clicked(QModelIndex)),this, SLOT(lll()));
//    }
//    if (sett->value("singleClick").toInt() == 2) {
//      connect(ui->viewIcon, SIGNAL(clicked(QModelIndex)),this, SLOT(listDoubleClicked(QModelIndex)));
//      connect(ui->viewtree, SIGNAL(clicked(QModelIndex)),this, SLOT(listDoubleClicked(QModelIndex)));
//    }

    // Connect ui->viewIcon view
    connect(ui->viewIcon, SIGNAL(activated(QModelIndex)),this, SLOT(listDoubleClicked(QModelIndex)));

    // Connect path edit
    connect(ui->pathEdit, SIGNAL(activated(QString)),this, SLOT(pathEditChanged(QString)));
    connect(customComplete, SIGNAL(activated(QString)),this, SLOT(pathEditChanged(QString)));
    connect(ui->pathEdit->lineEdit(), SIGNAL(cursorPositionChanged(int,int)),this, SLOT(addressChanged(int,int)));

    // Connect selection
    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)),this, SLOT(clipboardChanged()));
    connect(ui->viewDetail,SIGNAL(activated(QModelIndex)),this, SLOT(listDoubleClicked(QModelIndex)));
    connect(listSelectionModel,SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
            this, SLOT(listSelectionChanged(const QItemSelection,const QItemSelection)));

    // Connect copy progress
    connect(this, SIGNAL(copyProgressFinished(int,QStringList)),this, SLOT(progressFinished(int,QStringList)));

    // Conect ui->viewIcon model
    connect(modelList,SIGNAL(dragDropPaste(const QMimeData *, QString, myModel::DragMode)),
            this,SLOT(dragLauncher(const QMimeData *, QString, myModel::DragMode)));

    // Connect tabs
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(tabs, SIGNAL(dragDropTab(const QMimeData *, QString, QStringList)),
            this, SLOT(pasteLauncher(const QMimeData *, QString, QStringList)));
    connect(ui->viewIcon, SIGNAL(pressed(QModelIndex)),this, SLOT(listItemPressed(QModelIndex)));
    connect(ui->viewDetail, SIGNAL(pressed(QModelIndex)),this, SLOT(listItemPressed(QModelIndex)));
    connect(modelList, SIGNAL(thumbUpdate(QModelIndex)),this, SLOT(thumbUpdate(QModelIndex)));

    // Connect tool buttons to related actions
    connect(ui->zoomin,SIGNAL(pressed()),this,SLOT(zoomInAction()));
    connect(ui->zoomout,SIGNAL(pressed()),this,SLOT(zoomOutAction()));
    connect(ui->paste,SIGNAL(pressed()), this, SLOT(on_actionPaste_triggered()));
    connect(ui->refresh,SIGNAL(pressed()), this, SLOT(on_actionRefresh_triggered()));

    qApp->setKeyboardInputInterval(1000);

    // Setup sort files
    currentSortColumn = 0;
    currentSortOrder = Qt::AscendingOrder;
    switch (currentSortColumn) {
      case 0 : setSortColumn(ui->actionName); break;
      case 1 : setSortColumn(ui->actionSize); break;
      case 3 : setSortColumn(ui->actionDate); break;
    }
    modelView->sort(currentSortColumn,currentSortOrder);

    // Read defaults
    QTimer::singleShot(100, mimeUtils, SLOT(generateDefaults()));
    on_actionRefresh_triggered();
}

/**
 * @brief Loads application settings
 */
void corefm::loadSettings()
{
    // Load info whether use real mime types
    modelList->setRealMimeTypes(sm.getIsRealMimeType());

    // Load zoom settings
    zoom = sm.getZoomValue();
    zoomTree = sm.getZoomTreeValue();
    zoomList = sm.getZoomListValue();
    zoomDetail = sm.getZoomDetailValue();
    ui->viewDetail->setIconSize(QSize(zoomDetail, zoomDetail));
    ui->viewDir->setIconSize(QSize(zoomTree, zoomTree));

    // Load view settings
    ui->showthumb->setChecked(sm.getIsShowThumb());
    ui->showHidden->setChecked(sm.getShowHidden());
    on_showHidden_clicked(sm.getShowHidden());
    ui->Tools->setChecked(sm.getShowToolbox());
    ui->toolsBar->setVisible(sm.getShowToolbox());

    // Load view mode
    if (sm.getViewMode() == true) {
        viewMode(1);
        ui->viewMode->setChecked(1);
    }else if (sm.getViewMode() == false) {
        viewMode(0);
        ui->viewMode->setChecked(0);
    }

}

void corefm::closeEvent(QCloseEvent *event)
{
    event->ignore();

    // Save settings
    writeSettings();
    if (tabs->count() == 0) {
        // Function from globalfunctions.cpp
        saveToRecent("CoreFM", ui->pathEdit->currentText());
    }
    else if (tabs->count() > 0) {
        for (int i = 0; i < tabs->count(); i++) {
            tabs->setCurrentIndex(i);
            // Function from globalfunctions.cpp
            saveToRecent("CoreFM", ui->pathEdit->currentText());
        }
    }
    modelList->cacheInfo();
    event->accept();
}

/**
 * @brief Writes settings into config file
 */
void corefm::writeSettings()
{
    sm.setZoomValue(zoom);
    sm.setZoomTreeValue(zoomTree);
    sm.setZoomListValue(zoomList);
    sm.setZoomDetailValue(zoomDetail);
    sm.setIsShowThumb(ui->showthumb->isChecked());
    sm.setShowHidden(ui->showHidden->isChecked());
    sm.setViewMode(ui->viewMode->isChecked());
    sm.setShowToolbox(ui->toolsBar->isVisible());
    sm.setIsRealMimeType(modelList->isRealMimeTypes());
}

void corefm::treeSelectionChanged(QModelIndex current, QModelIndex previous)
{
    Q_UNUSED(previous);
    QFileInfo name = modelList->fileInfo(modelTree->mapToSource(current));
    if(!name.exists()) return;

    curIndex = name;

    if(ui->viewDir->hasFocus() && QApplication::mouseButtons() == Qt::MidButton)
    {
        listItemPressed(modelView->mapFromSource(modelList->index(name.filePath())));
        tabs->setCurrentIndex(tabs->count() - 1);
        if(currentView == 2) ui->viewDetail->setFocus(Qt::TabFocusReason);
        else ui->viewIcon->setFocus(Qt::TabFocusReason);
    }

    if(curIndex.filePath() != ui->pathEdit->itemText(0))
    {
        if(tabs->count()) tabs->addHistory(curIndex.filePath());
        ui->pathEdit->insertItem(0, curIndex.filePath());
        ui->pathEdit->setCurrentIndex(0);
    }

    if(modelList->setRootPath(name.filePath())) modelView->invalidate();

    QModelIndex baseIndex = modelView->mapFromSource(modelList->index(name.filePath()));

    if(currentView == 2) ui->viewDetail->setRootIndex(baseIndex);
    else ui->viewIcon->setRootIndex(baseIndex);

    // Set the tab text for root folder
    if (QFileInfo(curIndex.filePath()).isRoot())
        tabs->setTabText(tabs->currentIndex(), curIndex.filePath());

    if(tabs->count())
    {
        if (QFileInfo(curIndex.filePath()).isRoot())
            tabs->setTabText(tabs->currentIndex(), curIndex.filePath());
        else
            tabs->setTabText(tabs->currentIndex(), curIndex.fileName());

        tabs->setTabData(tabs->currentIndex(), curIndex.filePath());
        tabs->setIcon(tabs->currentIndex());
    }

    if(backIndex.isValid())
    {
        listSelectionModel->setCurrentIndex(modelView->mapFromSource(backIndex),QItemSelectionModel::ClearAndSelect);
        if(currentView == 2) ui->viewDetail->scrollTo(modelView->mapFromSource(backIndex));
        else ui->viewIcon->scrollTo(modelView->mapFromSource(backIndex));
    }
    else
    {
        listSelectionModel->blockSignals(1);
        listSelectionModel->clear();
    }

    listSelectionModel->blockSignals(0);
    QTimer::singleShot(30,this,SLOT(dirLoaded()));
}

void corefm::dirLoaded()
{
    if(backIndex.isValid()){
        backIndex = QModelIndex();
        return;
    }

    qint64 bytes = 0;
    QModelIndexList items;
    bool includeHidden = ui->showHidden->isChecked();


    for(int x = 0; x < modelList->rowCount(modelList->index(ui->pathEdit->currentText())); ++x)
        items.append(modelList->index(x,0,modelList->index(ui->pathEdit->currentText())));


    foreach(QModelIndex theItem,items)
    {
        if(includeHidden || !modelList->fileInfo(theItem).isHidden())
            bytes = bytes + modelList->size(theItem);
        else
            items.removeOne(theItem);
    }

    ui->totalitem->setText("Total : " + QString("%1 items").arg(items.count()));
    ui->selecteditem->clear();
    selectItemCount = 0;

    if(items.count() == 0){messageEngine("Folder is empty", MessageType::Info);}
    if(ui->showthumb->isChecked()) {
        modelList->setMode(true);
        QtConcurrent::run(modelList,&myModel::loadThumbs,items);
    }
}

void corefm::thumbUpdate(QModelIndex index)
{
    if(currentView == 2) ui->viewDetail->update(modelView->mapFromSource(index));
    else ui->viewIcon->update(modelView->mapFromSource(index));
}

void corefm::listSelectionChanged(const QItemSelection selected, const QItemSelection deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    QModelIndexList items;

    if(listSelectionModel->selectedRows(0).count()) items = listSelectionModel->selectedRows(0);
    else items = listSelectionModel->selectedIndexes();

    selectItemCount = 0 ;
    selectItemCount = items.count();

    ui->selecteditem->clear();
    if(selectItemCount == 0)
    {
        curIndex = ui->pathEdit->itemText(0);
        return;
    }

    curIndex = modelList->filePath(modelView->mapToSource(listSelectionModel->currentIndex()));

    QStringList selectedFilesPath;
    foreach (QModelIndex item, listSelectionModel->selectedIndexes()) {
        selectedFilesPath.append(modelList->filePath(modelView->mapToSource(item)));
    }

    if (selectItemCount == 1) {
       ui->name->setText(curIndex.fileName());
       // Function from globalfunctions.cpp
       ui->size->setText(formatSize(curIndex.size()));
    }else {
       ui->name->setText(ui->pathEdit->currentText());
       ui->size->setText(FileUtils::getMultipleFileSize(selectedFilesPath));
    }

    ui->selecteditem->setText("Selected : " + QString("%1 items").arg(items.count()));
}

void corefm::listItemClicked(QModelIndex current)
{
    if(modelList->filePath(modelView->mapToSource(current)) == ui->pathEdit->currentText()) return;

    Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
    if(mods == Qt::ControlModifier || mods == Qt::ShiftModifier) return;
    if(modelList->isDir(modelView->mapToSource(current)))
        ui->viewDir->setCurrentIndex(modelTree->mapFromSource(modelView->mapToSource(current)));
}

void corefm::listItemPressed(QModelIndex current)
{
    if(QApplication::mouseButtons() == Qt::MidButton){
        if(modelList->isDir(modelView->mapToSource(current)))
        {
            if(QApplication::keyboardModifiers() == Qt::ControlModifier)
                on_actionOpen_triggered();
            else
                addTab(modelList->filePath(modelView->mapToSource(current)));
        }
        else{
            on_actionOpen_triggered();
        }
    }
}

int corefm::addTab(const QString path)
{
    if (tabs->count() < 4) {
        if(tabs->count() == 0) tabs->addNewTab(ui->pathEdit->currentText(),currentView);
        return tabs->addNewTab(path,currentView);
    } else {
        messageEngine("Reached page limite", MessageType::Warning);
    }
    return -1;
}

void corefm::tabChanged(int index)
{
    if(tabs->count() == 0) return;

    ui->pathEdit->clear();
    ui->pathEdit->addItems(*tabs->getHistory(index));

//    int type = tabs->getType(index);
//    if(currentView != type)
//    {
//        if(type == 2) ui->viewMode->setChecked(0);
//        else ui->viewMode->setChecked(1);

//        if(type == 1) ui->viewMode->setChecked(1);
//        else ui->viewMode->setChecked(0);

//        viewMode(true);
//    }

    if(!tabs->tabData(index).toString().isEmpty())
        ui->viewDir->setCurrentIndex(modelTree->mapFromSource(modelList->index(tabs->tabData(index).toString())));
}

/**
 * @brief Doubleclick on icon/launcher
 * @param current
 */
void corefm::listDoubleClicked(QModelIndex current)
{
    Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
    if (mods == Qt::ControlModifier || mods == Qt::ShiftModifier) {
      return;
    }
    if (modelList->isDir(modelView->mapToSource(current))) {
      QModelIndex i = modelView->mapToSource(current);
      ui->viewDir->setCurrentIndex(modelTree->mapFromSource(i));
    } else {
      executeFile(current, 0);
    }
}

/**
 * @brief Reaction for change of path edit (location edit)
 * @param path
 */
void corefm::pathEditChanged(QString path)
{
    QString info = path;
    if (!QFileInfo(path).exists()) return;
    info.replace("~", QDir::homePath());
    ui->viewDir->setCurrentIndex(modelTree->mapFromSource(modelList->index(info)));
}

/**
 * @brief Reaction for change of clippboard content
 */
void corefm::clipboardChanged()
{
    if (QApplication::clipboard()->mimeData()->hasUrls()) {
      ui->actionPaste->setEnabled(1);
      ui->paste->setVisible(1);
    } else {
      modelList->clearCutItems();
      ui->actionPaste->setEnabled(0);
      ui->paste->setVisible(0);
    }
}

/**
 * @brief Drags data to the new location
 * @param data data to be pasted
 * @param newPath path of new location
 * @param dragMode mode of dragging
 */
void corefm::dragLauncher(const QMimeData *data, const QString &newPath,
                              myModel::DragMode dragMode) {
    // Retrieve urls (paths) of data
    QList<QUrl> files = data->urls();

    // If drag mode is unknown then ask what to do
    if (dragMode == myModel::DM_UNKNOWN) {
      QMessageBox box;
      box.setWindowTitle(tr("What do you want to do?"));
      box.setWindowIcon(QIcon(":/app/icons/app-icons/CoreFM.svg"));
      box.setStyleSheet(getStylesheetFileContent(":/appStyle/style/MessageBox.qss"));
      QAbstractButton *move = box.addButton(tr("Move here"), QMessageBox::ActionRole);
      QAbstractButton *copy = box.addButton(tr("Copy here"), QMessageBox::ActionRole);
      QAbstractButton *link = box.addButton(tr("Link here"), QMessageBox::ActionRole);
      QAbstractButton *canc = box.addButton(QMessageBox::Cancel);
      box.exec();
      if (box.clickedButton() == move) {
        dragMode = myModel::DM_MOVE;
      } else if (box.clickedButton() == copy) {
        dragMode = myModel::DM_COPY;
      } else if (box.clickedButton() == link) {
        dragMode = myModel::DM_LINK;
      } else if (box.clickedButton() == canc) {
        return;
      }
    }

    // If moving is enabled, cut files from the original location
    QStringList cutList;
    if (dragMode == myModel::DM_MOVE) {
      foreach (QUrl item, files) {
        cutList.append(item.path());
      }
    }

    // Paste launcher (this method has to be called instead of that with 'data'
    // parameter, because that 'data' can timeout)
    pasteLauncher(files, newPath, cutList, dragMode == myModel::DM_LINK);
}

/**
 * @brief Pastes data to the new location
 * @param data data to be pasted
 * @param newPath path of new location
 * @param cutList ui->viewIcon of items to remove
 */
void corefm::pasteLauncher(const QMimeData *data, const QString &newPath,
                               const QStringList &cutList) {
    QList<QUrl> files = data->urls();
    pasteLauncher(files, newPath, cutList);
}

/**
 * @brief Pastes files to the new path
 * @param files ui->viewIcon of files
 * @param newPath new path
 * @param cutList files to remove from original path
 * @param link true if link should be created (default value = false)
 */
void corefm::pasteLauncher(const QList<QUrl> &files, const QString &newPath,
                               const QStringList &cutList, bool link) {
    // File no longer exists?
    if (!QFile(files.at(0).path()).exists()) {
      QString msg = tr("File '%1' no longer exists!").arg(files.at(0).path());
      messageEngine(msg, MessageType::Info);
      ui->actionPaste->setEnabled(0);
      ui->paste->setVisible(0);
      return;
    }

    // Temporary variables
    int replace = 0;
    QStringList completeList;
    QString baseName = QFileInfo(files.at(0).toLocalFile()).path();

    // Only if not in same directory, otherwise we will do 'Copy(x) of'
    if (newPath != baseName) {

      foreach (QUrl file, files) {

        // Merge or replace?
        QFileInfo temp(file.toLocalFile());

        if (temp.isDir() && QFileInfo(newPath + QDir::separator() + temp.fileName()).exists()) {
          QString msg = QString("<b>%1</b><p>Already exists!<p>What do you want to do?").arg(newPath + QDir::separator() + temp.fileName());
          QMessageBox message(QMessageBox::Question, tr("Existing folder"), msg, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
          message.setWindowIcon(QIcon(":/app/icons/app-icons/CoreFM.svg"));
          message.setStyleSheet(getStylesheetFileContent(":/appStyle/style/MessageBox.qss"));
          message.button(QMessageBox::Yes)->setText(tr("Merge"));
          message.button(QMessageBox::No)->setText(tr("Replace"));

          int merge = message.exec();
          if (merge == QMessageBox::Cancel) return;
          if (merge == QMessageBox::Yes) {
            FileUtils::recurseFolder(temp.filePath(), temp.fileName(), &completeList);
          }
          else {
            FileUtils::removeRecurse(newPath, temp.fileName());
          }
        }
        else completeList.append(temp.fileName());
      }

      // Ask whether replace files if files with same name already exist in
      // destination directory
      foreach (QString file, completeList) {
        QFileInfo temp(newPath + QDir::separator() + file);
        if (temp.exists()) {
          QFileInfo orig(baseName + QDir::separator() + file);
          if (replace != QMessageBox::YesToAll && replace != QMessageBox::NoToAll) {
            // TODO: error dispalys only at once
            replace = showReplaceMsgBox(temp, orig);
          }
          if (replace == QMessageBox::Cancel) {
            return;
          }
          if (replace == QMessageBox::Yes || replace == QMessageBox::YesToAll) {
            QFile(temp.filePath()).remove();
          }
        }
      }
    }

    // If only links should be created, create them and exit
    if (link) {
      linkFiles(files, newPath);
      return;
    }

    // Copy/move files
    QString title = cutList.count() == 0 ? tr("Copying...") : tr("Moving...");
    progress = new myProgressDialog(title);
    connect(this, SIGNAL(updateCopyProgress(qint64, qint64, QString)), progress, SLOT(update(qint64, qint64, QString)));
    listSelectionModel->clear();
    QtConcurrent::run(this, &corefm::pasteFiles, files, newPath, cutList);
}

/**
 * @brief Pastes ui->viewIcon of files/dirs into new path
 * @param files ui->viewIcon of files
 * @param newPath new (destination) path
 * @param cutList ui->viewIcon of files that are going to be removed from source path
 * @return true if operation was successfull
 */
bool corefm::pasteFiles(const QList<QUrl> &files, const QString &newPath,const QStringList &cutList)
{
    // Temporary variables
    bool ok = true;
    QStringList newFiles;

    // Quit if folder not writable
    if (!QFileInfo(newPath).isWritable()
        || newPath == QDir(files.at(0).toLocalFile()).path()) {
      emit copyProgressFinished(1, newFiles);
      return 0;
    }

    // Get total size in bytes
    qint64 total = FileUtils::totalSize(files);

    // Check available space on destination before we start
    struct statfs info;
    statfs(newPath.toLocal8Bit(), &info);
    if ((qint64) info.f_bavail * info.f_bsize < total) {

      // If it is a cut/move on the same device it doesn't matter
      if (cutList.count()) {
        qint64 driveSize = (qint64) info.f_bavail*info.f_bsize;
        statfs(files.at(0).path().toLocal8Bit(),&info);

        // Same device?
        if ((qint64) info.f_bavail*info.f_bsize != driveSize) {
          emit copyProgressFinished(2, newFiles);
          return 0;
        }
      } else {
        emit copyProgressFinished(2, newFiles);
        return 0;
      }
    }

    // Main loop
    for (int i = 0; i < files.count(); ++i) {

      // Canceled ?
      if (progress->result() == 1) {
        emit copyProgressFinished(0, newFiles);
        return 1;
      }

      // Destination file name and url
      QFileInfo temp(files.at(i).toLocalFile());
      QString destName = temp.fileName();
      QString destUrl = newPath + QDir::separator() + destName;

      // Only do 'Copy(x) of' if same folder
      if (temp.path() == newPath) {
        int num = 1;
        while (QFile(destUrl).exists()) {
          destName = QString("Copy (%1) of %2").arg(num).arg(temp.fileName());
          destUrl = newPath + QDir::separator() + destName;
          num++;
        }
      }

      // If destination file does not exist and is directory
      QFileInfo dName(destUrl);
      if (!dName.exists() || dName.isDir()) {

        // Keep a ui->viewIcon of new files so we can select them later
        newFiles.append(destUrl);

        // Cut action
        if (cutList.contains(temp.filePath())) {

          // Files or directories
          if (temp.isFile()) {

            // NOTE: Rename will fail if across different filesystem
            /*QFSFileEngine*/ QFile file(temp.filePath());
            if (!file.rename(destUrl))	{
              ok = cutCopyFile(temp.filePath(), destUrl, total, true);
            }
          } else {
            ok = QFile(temp.filePath()).rename(destUrl);

            // File exists or move folder between different filesystems, so use
            // copy/remove method
            if (!ok) {
              if (temp.isDir()) {
                ok = true;
                copyFolder(temp.filePath(), destUrl, total, true);
                modelList->clearCutItems();
              }
              // File already exists, don't do anything
            }
          }
        } else {
          if (temp.isDir()) {
            copyFolder(temp.filePath(),destUrl,total,false);
          } else {
            ok = cutCopyFile(temp.filePath(), destUrl, total, false);
          }
        }
      }
    }
//    qDebug()<< "pasteFiles";

    // Finished
    emit copyProgressFinished(0, newFiles);
    return 1;
}

/**
 * @brief Copies source directory to destination directory
 * @param srcFolder location of source directory
 * @param dstFolder location of destination directory
 * @param total total copy size
 * @param cut true/false if source directory is going to be moved/copied
 * @return true if copy was successfull
 */
bool corefm::copyFolder(const QString &srcFolder, const QString &dstFolder,qint64 total, bool cut)
{
    // Temporary variables
    QDir srcDir(srcFolder);
    QDir dstDir(QFileInfo(dstFolder).path());
    QStringList files;
    bool ok = true;

    // Name of destination directory
    QString folderName = QFileInfo(dstFolder).fileName();

    // Id destination location does not exist, create it
    if (!QFileInfo(dstFolder).exists()) {
      dstDir.mkdir(folderName);
    }
    dstDir = QDir(dstFolder);

    // Get files in source directory
    files = srcDir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);

    // Copy each file
    for (int i = 0; i < files.count(); i++) {
      QString srcName = srcDir.path() + QDir::separator() + files[i];
      QString dstName = dstDir.path() + QDir::separator() + files[i];

      // Don't remove source folder if all files not cut
      if (!cutCopyFile(srcName, dstName, total, cut)) ok = false;

      // Cancelled
      if (progress->result() == 1) return 0;
    }

    // Get directories in source directory
    files.clear();
    files = srcDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden);

    // Copy each directory
    for (int i = 0; i < files.count(); i++) {
      if (progress->result() == 1) {
        return 0;
      }
      QString srcName = srcDir.path() + QDir::separator() + files[i];
      QString dstName = dstDir.path() + QDir::separator() + files[i];
      copyFolder(srcName, dstName, total, cut);
    }

    // Remove source folder if all files moved ok
    if (cut && ok) {
      srcDir.rmdir(srcFolder);
    }
//    qDebug()<< "copyFolder";
    return ok;
}

/**
 * @brief Copies or moves file
 * @param src location of source file
 * @param dst location of destination file
 * @param totalSize total copy size
 * @param cut true/false if source file is going to be moved/copied
 * @return true if copy was successfull
 */
bool corefm::cutCopyFile(const QString &src, QString dst, qint64 totalSize,bool cut)
{
    // Create files with given locations
    QFile srcFile(src);
    QFile dstFile(dst);

    // Destination file already exists, exit
    if (dstFile.exists()) return 1;

    // If destination location is too long make it shorter
    if (dst.length() > 50) dst = "/.../" + dst.split(QDir::separator()).last();

    // Open source and destination files
    srcFile.open(QFile::ReadOnly);
    dstFile.open(QFile::WriteOnly);

    // Determine buffer size, calculate size of file and number of steps
    char block[4096];
    qint64 total = srcFile.size();
    qint64 steps = total >> 7; // shift right 7, same as divide 128, much faster
    qint64 interTotal = 0;

    // Copy blocks
    while (!srcFile.atEnd()) {
      if (progress->result() == 1) break; // cancelled
      qint64 inBytes = srcFile.read(block, sizeof(block));
      dstFile.write(block, inBytes);
      interTotal += inBytes;
      if (interTotal > steps) {
        emit updateCopyProgress(interTotal, totalSize, dst);
        interTotal = 0;
      }
    }

    // Update copy progress
    emit updateCopyProgress(interTotal, totalSize, dst);

    dstFile.close();
    srcFile.close();

    if (dstFile.size() != total) return 0;
    if (cut) srcFile.remove();  // if file is cut remove the source
//    qDebug()<< "cutCopyFile";
    return 1;
}

/**
 * @brief Creates symbolic links to files
 * @param files
 * @param newPath
 * @return true if link creation was successfull
 */
bool corefm::linkFiles(const QList<QUrl> &files, const QString &newPath)
{
    // Quit if folder not writable
    if (!QFileInfo(newPath).isWritable()
        || newPath == QDir(files.at(0).toLocalFile()).path()) {
      return false;
    }

    // TODO: even if symlinks are small we have to make sure that we have space
    // available for links

    // Main loop
    for (int i = 0; i < files.count(); ++i) {

      // Choose destination file name and url
      QFile file(files.at(i).toLocalFile());
      QFileInfo temp(file);
      QString destName = temp.fileName();
      QString destUrl = newPath + QDir::separator() + destName;

      // Only do 'Link(x) of' if same folder
      if (temp.path() == newPath) {
        int num = 1;
        while (QFile(destUrl).exists()) {
          destName = QString("Link (%1) of %2").arg(num).arg(temp.fileName());
          destUrl = newPath + QDir::separator() + destName;
          num++;
        }
      }

      // If file does not exists then create link
      QFileInfo dName(destUrl);
      if (!dName.exists()) {
        file.link(destUrl);
      }
    }
//    qDebug()<< "linkFiles";
    return true;
}

/**
 * @brief Asks user whether replace file 'f1' with another file 'f2'
 * @param f1 file to be replaced with f2
 * @param f2 file to replace f1
 * @return result
 */
int corefm::showReplaceMsgBox(const QFileInfo &f1, const QFileInfo &f2)
{
    // Create message
    QString t = tr("Do you want to replace:<p><b>%1</p><p>Modified: %2<br>"
                   "Size: %3 bytes</p><p>with:<p><b>%4</p><p>Modified: %5"
                   "<br>Size: %6 bytes</p>");

    // Populate message with data
    t = t.arg(f1.filePath()).arg(f1.lastModified().toString()).arg(f1.size())
         .arg(f2.filePath()).arg(f2.lastModified().toString()).arg(f2.size());

    // Show message
    QMessageBox message(QMessageBox::Question, tr("Replace"), t, QMessageBox::Yes
                                 | QMessageBox::YesToAll | QMessageBox::No
                                 | QMessageBox::NoToAll | QMessageBox::Cancel);
    message.setWindowIcon(QIcon(":/app/icons/app-icons/CoreFM.svg"));
    message.setStyleSheet(getStylesheetFileContent(":/appStyle/style/MessageBox.qss"));

    return message.exec();
}

void corefm::progressFinished(int ret,QStringList newFiles)
{
    if(progress != 0)
    {
        progress->close();
        delete progress;
        progress = 0;
    }

    if(newFiles.count())
    {
        disconnect(listSelectionModel,SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),this,SLOT(listSelectionChanged(const QItemSelection, const QItemSelection)));

        qApp->processEvents();              //make sure notifier has added new files to the model

        if(QFileInfo(newFiles.first()).path() == ui->pathEdit->currentText())       //highlight new files if visible
        {
            foreach(QString item, newFiles)
                listSelectionModel->select(modelView->mapFromSource(modelList->index(item)),QItemSelectionModel::Select);
        }

        connect(listSelectionModel,SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),this,SLOT(listSelectionChanged(const QItemSelection, const QItemSelection)));
        curIndex.setFile(newFiles.first());

        if(currentView == 2) ui->viewDetail->scrollTo(modelView->mapFromSource(modelList->index(newFiles.first())),QAbstractItemView::EnsureVisible);
        else ui->viewIcon->scrollTo(modelView->mapFromSource(modelList->index(newFiles.first())),QAbstractItemView::EnsureVisible);

        if(QFile(QDir::tempPath() + "/corefm.temp").exists()) QApplication::clipboard()->clear();

        clearCutItems();
    }

    if(ret == 1) messageEngine("Paste failed\nDo you have write permissions?",MessageType::Warning);
    if(ret == 2) messageEngine("Too big!\nThere is not enough space!",MessageType::Warning);
}


/**
 * @brief Creates menu for opening file in selected application
 * @return menu
 */
QMenu* corefm::createOpenWithMenu()
{
    QMenu *openMenu = new QMenu(tr("Open with"));

    // Adding CoreApps
    openMenu->addAction(ui->actionCoreImage);
    openMenu->addAction(ui->actionCorePaint);
    openMenu->addAction(ui->actionCorePad);
    openMenu->addAction(ui->actionCorePlayer);
    openMenu->addAction(ui->actionCorePDF);

    // Select action
    QAction *selectAppAct = new QAction(tr("Select..."), openMenu);
    connect(selectAppAct, SIGNAL(triggered()), this, SLOT(selectApp()));

    // Load default applications for current mime
    QMimeDatabase mimetype;
    QString mime = mimetype.mimeTypeForFile(curIndex.filePath()).name();
    QStringList appNames = mimeUtils->getDefault(mime);

    // Create actions for opening
    QList<QAction*> defaultApps;
    foreach (QString appName, appNames) {

      // Skip empty app name
      if (appName.isEmpty()) {
        continue;
      }

      // Load desktop file for application
      DesktopFile df = DesktopFile("/usr/share/applications/" + appName);

      // Create action
      QAction* action = new QAction(df.getName(), openMenu);
      action->setData(df.getExec());
      action->setIcon(FileUtils::searchAppIcon(df));
      defaultApps.append(action);

      // TODO: icon and connect
      connect(action, SIGNAL(triggered()), SLOT(openInApp()));

      // Add action to menu
      openMenu->addAction(action);
    }

    // Add open action to menu
    openMenu->addSeparator();
    openMenu->addAction(selectAppAct);
    return openMenu;
}

QMenu* corefm::sendto()
{
    QMenu *sendto = new QMenu(tr("Send to.."), this);

    sendto->addAction(ui->actionDesktop);
    sendto->addAction(ui->actionHome);

    const auto allMounted = QStorageInfo::mountedVolumes();
    for(auto& singleMounted : allMounted){
        if(singleMounted.device() != "tmpfs" && singleMounted.rootPath() != "/") {
            QAction *action = new QAction(singleMounted.displayName(),sendto);
            action->setData(singleMounted.displayName());
            action->setIcon(QIcon(":/icons/drive.svg"));
            sendto->addAction(action);
            connect(action,SIGNAL(triggered()),SLOT(sendToPath()));
        }
    }
    return sendto;
}

QMenu* corefm::globalmenu(){

    QMenu *popup = new QMenu(this);
    QMenu *subnew = new QMenu(tr("New.."), this);
    QMenu *innew = new QMenu(tr("Open in.."), this);
    QMenu *arrageItems = new QMenu(tr("Arrage Items"), this);

    QFile file(curIndex.filePath());
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(file, QMimeDatabase::MatchContent);
    QString extrainfo = mime.name();

    sortByActGrp = new QActionGroup(this);
    sortByActGrp->addAction(ui->actionName);
    sortByActGrp->addAction(ui->actionDate);
    sortByActGrp->addAction(ui->actionSize);
    connect(sortByActGrp, SIGNAL(triggered(QAction*)), SLOT(toggleSortBy(QAction*)));

    innew->addAction(ui->actionNewPage);
    innew->addAction(ui->actionCoreFM);
    innew->addAction(ui->actionCoreRenamer);

    subnew->addAction(ui->actionNewFolder);
    subnew->addAction(ui->actionNewTextFile);

    arrageItems->addAction(ui->actionName);
    arrageItems->addAction(ui->actionSize);
    arrageItems->addAction(ui->actionDate);
    arrageItems->addSeparator();
    arrageItems->addAction(ui->actionAscending);

    //Detect whether this is the Trash folder because menus are different
    if (ui->pathEdit->currentText() == QDir::homePath() + "/.local/share/Trash/files") { //This is the Trash folder
        popup->addSection("Nothing");
        return popup;
    }

    if (selectItemCount > 1) { // multipal file
        popup->addSeparator();
        popup->addMenu(sendto());
        popup->addSeparator();
        popup->addAction(ui->actionCut);
        popup->addAction(ui->actionCopy);
        popup->addSeparator();
        popup->addAction(ui->actionTrash_it);
        popup->addSeparator();
    }

    else  if(selectItemCount == 1){
      if(curIndex.isFile()){ // file
          popup->addAction(ui->actionOpen);
          popup->addMenu(createOpenWithMenu());
          popup->addAction(ui->actionRun);
          popup->addSeparator();
          popup->addMenu(sendto());
          popup->addSeparator();
          popup->addAction(ui->actionCut);
          popup->addAction(ui->actionCopy);
          popup->addSeparator();
          popup->addAction(ui->actionRename);
          popup->addAction(ui->actionTrash_it);
          popup->addSeparator();
          popup->addAction(ui->actionCreate_Archive);
          if(extrainfo.contains("application")){
              popup->addAction(ui->actionExtract_Here);
          }
          popup->addSeparator();
          popup->addSeparator();
          popup->addAction(ui->actionProperties);
        }
      if(curIndex.isDir()){ // folder
          popup->addAction(ui->actionOpen);
          popup->addMenu(innew);
          popup->addMenu(createOpenWithMenu());
          popup->addSeparator();
          popup->addMenu(sendto());
          popup->addSeparator();
          popup->addAction(ui->actionCut);
          popup->addAction(ui->actionCopy);
          popup->addAction(ui->actionPaste);
          popup->addSeparator();
          popup->addAction(ui->actionRename);
          popup->addAction(ui->actionTrash_it);
          popup->addSeparator();
          popup->addAction(ui->actionTerminal);
          popup->addAction(ui->actionCreate_Archive);
          if(extrainfo.contains("application")){
              popup->addAction(ui->actionExtract_Here);
          }
          popup->addSeparator();
          popup->addSeparator();
          popup->addAction(ui->actionProperties);
        }
    }
    else{ // whitespace
        on_actionRefresh_triggered();
        popup->addAction(ui->actionSelectAll);
        popup->addSeparator();
        popup->addMenu(arrageItems);
        popup->addAction(ui->actionRefresh);
        popup->addSeparator();
        if (QApplication::clipboard()->mimeData()->hasUrls()) {
            popup->addAction(ui->actionPaste);
        }
        popup->addSeparator();
        popup->addMenu(subnew);
        popup->addAction(ui->actionItemsToText);
        popup->addAction(ui->actionTerminal);
        popup->addSeparator();
        popup->addAction(ui->actionProperties);
    }
    return popup;
}

/**
 * @brief Selects application for opening file
 */
void corefm::selectApp()
{
    // Select application in the dialog
    ApplicationDialog *dialog = new ApplicationDialog(this);
    if (dialog->exec()) {
      if (dialog->getCurrentLauncher().compare("") != 0) {
        QString appName = dialog->getCurrentLauncher() + ".desktop";
        DesktopFile df = DesktopFile("/usr/share/applications/" + appName);
        mimeUtils->openInApp(df.getExec(), curIndex, this);
      }
    }
}

/**
 * @brief Opens file in application
 */
void corefm::openInApp()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if (action) {
      mimeUtils->openInApp(action->data().toString(), curIndex, this);
    }
}

void corefm::actionMapper(QString cmd)
{
    QModelIndexList selList;
    QStringList temp;

    if(focusWidget() == ui->viewIcon || focusWidget() == ui->viewDetail)
    {
        QFileInfo file = modelList->fileInfo(modelView->mapToSource(listSelectionModel->currentIndex()));

        if(file.isDir())
            cmd.replace("%n",file.fileName().replace(" ","\\"));
        else
            cmd.replace("%n",file.baseName().replace(" ","\\"));

        if(listSelectionModel->selectedRows(0).count()) selList = listSelectionModel->selectedRows(0);
        else selList = listSelectionModel->selectedIndexes();
    }
    else
        selList << modelView->mapFromSource(modelList->index(curIndex.filePath()));


    cmd.replace("~",QDir::homePath());


    //process any input tokens
    int pos = 0;
    while(pos >= 0)
    {
        pos = cmd.indexOf("%i",pos);
        if(pos != -1)
        {
            pos += 2;
            QString var = cmd.mid(pos,cmd.indexOf(" ",pos) - pos);
            QString input = QInputDialog::getText(this,tr("Input"), var, QLineEdit::Normal);
            if(input.isNull()) return;              //cancelled
            else cmd.replace("%i" + var,input);
        }
    }


    foreach(QModelIndex index,selList)
        temp.append(modelList->fileName(modelView->mapToSource(index)).replace(" ","\\"));

    cmd.replace("%f",temp.join(" "));

    temp.clear();

    foreach(QModelIndex index,selList)
        temp.append(modelList->filePath(modelView->mapToSource(index)).replace(" ","\\"));

    cmd.replace("%F",temp.join(" "));
}


void corefm::clearCutItems()
{
    //this refreshes existing items, sizes etc but doesn't re-sort
    modelList->clearCutItems();
    modelList->update();

    QModelIndex baseIndex = modelView->mapFromSource(modelList->index(ui->pathEdit->currentText()));

    if(currentView == 2) ui->viewDetail->setRootIndex(baseIndex);
    else ui->viewIcon->setRootIndex(baseIndex);
    QTimer::singleShot(50,this,SLOT(dirLoaded()));
    return;
}

bool mainTreeFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    myModel* fileModel = qobject_cast<myModel*>(sourceModel());

    if(fileModel->isDir(index0))
        if(this->filterRegExp().isEmpty() || fileModel->fileInfo(index0).isHidden() == 0) return true;

    return false;
}

bool viewsSortProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(this->filterRegExp().isEmpty()) return true;

    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    myModel* fileModel = qobject_cast<myModel*>(sourceModel());

    if(fileModel->fileInfo(index0).isHidden()) return false;
    else return true;
}

bool viewsSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    myModel* fsModel = dynamic_cast<myModel*>(sourceModel());

    if((fsModel->isDir(left) && !fsModel->isDir(right)))
        return sortOrder() == Qt::AscendingOrder;
    else if(!fsModel->isDir(left) && fsModel->isDir(right))
        return sortOrder() == Qt::DescendingOrder;

    if(left.column() == 1)          //size
    {
        if(fsModel->size(left) > fsModel->size(right)) return true;
        else return false;
    }
    else
    if(left.column() == 3)          //date
    {
        if(fsModel->fileInfo(left).lastModified() > fsModel->fileInfo(right).lastModified()) return true;
        else return false;
    }

    return QSortFilterProxyModel::lessThan(left,right);
}

QStringList myCompleter::splitPath(const QString& path) const
{
    QStringList parts = path.split("/");
    parts[0] = "/";

    return parts;
}

QString myCompleter::pathFromIndex(const QModelIndex& index) const
{
    if(!index.isValid()) return "";

    QModelIndex idx = index;
    QStringList list;
    do
    {
        QString t = model()->data(idx, Qt::EditRole).toString();
        list.prepend(t);
        QModelIndex parent = idx.parent();
        idx = parent.sibling(parent.row(), index.column());
    }
    while (idx.isValid());

    list[0].clear() ; // the join below will provide the separator

    return list.join("/");
}

//void corefm::focusAction()//delete
//{
//    QAction *which = qobject_cast<QAction*>(sender());
//    if(which)
//    {
//        if(which->text().contains("address")) ui->pathEdit->setFocus(Qt::TabFocusReason);
//        else if(which->text().contains("tree")) ui->viewDir->setFocus(Qt::TabFocusReason);
//        else if(currentView == 2) ui->viewDetail->setFocus(Qt::TabFocusReason);
//        else ui->viewIcon->setFocus(Qt::TabFocusReason);
//    }
//    else
//    {
//        QApplication::clipboard()->blockSignals(0);
//        ui->pathEdit->setCompleter(customComplete);
//    }
//}

void corefm::addressChanged(int old, int now)
{
    Q_UNUSED(old);
    if(!ui->pathEdit->hasFocus()) return;
    QString temp = ui->pathEdit->currentText();

    if(temp.contains("/."))
        if(!ui->showHidden->isChecked())
        {
            ui->showHidden->setChecked(1);
            on_showHidden_clicked(true);
        }

    if(temp.right(1) == "/")
    {
        modelList->index(temp);     //make sure model has walked this folder
        modelTree->invalidate();
    }

    if(temp.length() == now) return;
    int pos = temp.indexOf("/", now);

    ui->pathEdit->lineEdit()->blockSignals(1);

    if(QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        ui->viewDir->setCurrentIndex(modelTree->mapFromSource(modelList->index(temp.left(pos))));
    }
    else
    if(QApplication::mouseButtons() == Qt::MidButton)
    {
        QApplication::clipboard()->blockSignals(1);
        QApplication::clipboard()->clear(QClipboard::Selection);        //don't paste stuff

        ui->pathEdit->setCompleter(0);
        ui->viewDir->setCurrentIndex(modelTree->mapFromSource(modelList->index(temp.left(pos))));
    }
    else
    if(!ui->pathEdit->lineEdit()->hasSelectedText())
    {
        ui->pathEdit->completer()->setCompletionPrefix(temp.left(pos) + "/");
        ui->pathEdit->completer()->complete();
    }

    ui->pathEdit->lineEdit()->blockSignals(0);
}

void corefm::zoomInAction()
{
    int zoomLevel;

    if(focusWidget() == ui->viewDir)
    {
        (zoomTree == 64) ? zoomTree=64 : zoomTree+= 8;
        ui->viewDir->setIconSize(QSize(zoomTree,zoomTree));
        zoomLevel = zoomTree;
    }
    else
    {
        if(ui->view->currentIndex() == 0)
        {
            if(ui->viewMode->isChecked())
            {
                (zoom == 128) ? zoom=128 : zoom+= 8;
                zoomLevel = zoom;
            }
            else
            {
                (zoomList == 128) ? zoomList=128 : zoomList+= 8;
                zoomLevel = zoomList;
            }
            viewMode(true);
//            on_icon_clicked(true);
        }
        else
        {
            (zoomDetail == 64) ? zoomDetail=64 : zoomDetail+= 8;
            ui->viewDetail->setIconSize(QSize(zoomDetail, zoomDetail));
            zoomLevel = zoomDetail;
        }
    }

    // Function from globalfunctions.cpp
    messageEngine(QString(tr("Zoom: %1")).arg(zoomLevel), MessageType::Info);
}

void corefm::zoomOutAction()
{
    int zoomLevel;

    if(focusWidget() == ui->viewDir)
    {
        (zoomTree == 16) ? zoomTree=16 : zoomTree-= 8;
        ui->viewDir->setIconSize(QSize(zoomTree,zoomTree));
        zoomLevel = zoomTree;
    }
    else
    {
        if(ui->view->currentIndex() == 0)
        {
            if(ui->viewMode->isChecked())
            {
                (zoom == 16) ? zoom=16 : zoom-= 8;
                zoomLevel = zoom;
            }
            else
            {
                (zoomList == 16) ? zoomList=16 : zoomList-= 8;
                zoomLevel = zoomList;
            }
            viewMode(true);
//            on_icon_clicked(true);
        }
        else
        {
            (zoomDetail == 16) ? zoomDetail=16 : zoomDetail-= 8;
            ui->viewDetail->setIconSize(QSize(zoomDetail,zoomDetail));
            zoomLevel = zoomDetail;
        }
    }

    // Function from globalfunctions.cpp
    messageEngine(QString(tr("Zoom: %1")).arg(zoomLevel), MessageType::Info);
}

void corefm::on_actionRename_triggered()
{
    renameDialog *rd = new renameDialog(curIndex, this);
    rd->show();
}

void corefm::on_actionOpen_triggered()
{
    //openFile
//    QModelIndexList items;
//    if (listSelectionModel->selectedRows(0).count()) {
//      items = listSelectionModel->selectedRows(0);
//    } else {
//      items = listSelectionModel->selectedIndexes();
//    }

    listDoubleClicked(listSelectionModel->currentIndex());


    // Executes each file of selection
    //foreach (QModelIndex index, items) {
    //    executeFile(index, 0);
    //}

    //openfolder
    //QModelIndex i = listSelectionModel->currentIndex();
    //tree->setCurrentIndex(modelTree->mapFromSource(i));
}

void corefm::on_actionDelete_triggered()
{
    // Temporary selection info
    QModelIndexList selList;
    bool yesToAll = false;

    // Retrieves selection
    if (focusWidget() == ui->viewDir) {
      selList << modelList->index(ui->pathEdit->itemText(0));
    }
    else {
      QModelIndexList proxyList;
      if (listSelectionModel->selectedRows(0).count()) {
        proxyList = listSelectionModel->selectedRows(0);
      }
      else {
        proxyList = listSelectionModel->selectedIndexes();
      }
      foreach (QModelIndex proxyItem, proxyList) {
        selList.append(modelView->mapToSource(proxyItem));
      }
    }

    bool ok = false;

    // Delete selected file(s)
    for (int i = 0; i < selList.count(); ++i) {
      QFileInfo file(modelList->filePath(selList.at(i)));
      if (file.isWritable()) {
        if (file.isSymLink()) {
          ok = QFile::remove(file.filePath());
        }
        else {
          if (yesToAll == false) {
              QString title = tr("Careful");
              QString msg = tr("Are you sure you want to delete <p><b>\"") + file.filePath() + "</b>?";
              QMessageBox message(QMessageBox::Question, title, msg,QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll);
              message.setWindowIcon(QIcon(":/app/icons/app-icons/CoreFM.svg"));
              message.setStyleSheet(getStylesheetFileContent(":/appStyle/style/MessageBox.qss"));
              int ret = message.exec();
              if (ret == QMessageBox::YesToAll) yesToAll = true;
              if (ret == QMessageBox::No) return;
          }
          ok = modelList->remove(selList.at(i));
        }
      }
      else if (file.isSymLink()) {
        ok = QFile::remove(file.filePath());
      }
    }

    // Display error message if deletion failed
    if(!ok) {
        // Function from globalfunctions.cpp
        QString msg = tr("Could not delete some items...do you have the permissions?");
        messageEngine(msg, MessageType::Warning);
    }

    return;
}

void corefm::on_actionBack_triggered()
{
    // If there is only one item in path edit, we cannot go back
    if (ui->pathEdit->count() == 1) return;

    // Retrieve current index
    QString current = ui->pathEdit->currentText();
    if (current.contains(ui->pathEdit->itemText(1))) {
      backIndex = modelList->index(current);
    }

    // Remove history
    do {
      ui->pathEdit->removeItem(0);
      if (tabs->count()) tabs->remHistory();
    } while (!QFileInfo(ui->pathEdit->itemText(0)).exists()
             || ui->pathEdit->itemText(0) == current);

    // Sets new dir index
    QModelIndex i = modelList->index(ui->pathEdit->itemText(0));
    ui->viewDir->setCurrentIndex(modelTree->mapFromSource(i));
}

void corefm::on_actionUp_triggered()
{
    ui->viewDir->setCurrentIndex(ui->viewDir->currentIndex().parent());
}

void corefm::on_actionCut_triggered()
{
    // Temporary selection files
    QModelIndexList selList;
    QStringList fileList;

    // Selection
    if (focusWidget() == ui->viewDir) {
      selList << modelView->mapFromSource(modelList->index(ui->pathEdit->itemText(0)));
    } else if (listSelectionModel->selectedRows(0).count()) {
      selList = listSelectionModel->selectedRows(0);
    } else {
      selList = listSelectionModel->selectedIndexes();
    }

    // Retrieve selected indices
    foreach (QModelIndex item, selList) {
      fileList.append(modelList->filePath(modelView->mapToSource(item)));
    }

    clearCutItems();
    modelList->addCutItems(fileList);

    // Save a temp file to allow pasting in a different instance
    QFile tempFile(QDir::tempPath() + "/corefm.temp");
    tempFile.open(QIODevice::WriteOnly);
    QDataStream out(&tempFile);
    out << fileList;
    tempFile.close();

    QApplication::clipboard()->setMimeData(modelView->mimeData(selList));

    modelTree->invalidate();
    listSelectionModel->clear();
    ui->paste->setVisible(true);
    // Function from globalfunctions.cpp
    messageEngine("File moves successfully.", MessageType::Info);
}

void corefm::on_actionCopy_triggered()
{
    // Selection
    QModelIndexList selList;
    if (listSelectionModel->selectedRows(0).count()) {
      selList = listSelectionModel->selectedRows(0);
    } else {
      selList = listSelectionModel->selectedIndexes();
    }

    if (selList.count() == 0) {
      if (focusWidget() == ui->viewDir) {
        QModelIndex i = modelList->index(ui->pathEdit->itemText(0));
        selList << modelView->mapFromSource(i);
      } else {
        return;
      }
    }

    clearCutItems();

    QStringList text;
    foreach (QModelIndex item,selList) {
      text.append(modelList->filePath(modelView->mapToSource(item)));
    }

    QApplication::clipboard()->setText(text.join("\n"), QClipboard::Selection);
    QApplication::clipboard()->setMimeData(modelView->mimeData(selList));

    ui->actionCut->setData(0);
    ui->paste->setVisible(true);
}

void corefm::on_actionPaste_triggered()
{
    QString newPath;
    QStringList cutList;

    if (curIndex.isDir()) newPath = curIndex.filePath();
    else newPath = ui->pathEdit->itemText(0);

    // Check ui->viewIcon of files that are to be cut
    QFile tempFile(QDir::tempPath() + "/corefm.temp");
    if (tempFile.exists()) {
      tempFile.open(QIODevice::ReadOnly);
      QDataStream out(&tempFile);
      out >> cutList;
      tempFile.close();
    }
    pasteLauncher(QApplication::clipboard()->mimeData(), newPath, cutList);
    ui->paste->setVisible(false);
    on_actionRefresh_triggered();
    // Function from globalfunctions.cpp
    messageEngine("Paste Completed.", MessageType::Info);
}

void corefm::on_actionProperties_triggered()
{
    const QString path(curIndex.filePath());
    properties = new propertiesw(path);
}

void corefm::on_actionRefresh_triggered()
{
//    QApplication::clipboard()->clear();
    listSelectionModel->clear();

    modelList->refreshItems();
    //modelTree->invalidate();
    //modelTree->sort(0, Qt::AscendingOrder);
    modelView->invalidate();
    dirLoaded();

    return;
}

void corefm::on_actionNewFolder_triggered()
{
    // Check whether current directory is writeable
    QModelIndex newDir;
    if (!QFileInfo(ui->pathEdit->itemText(0)).isWritable()) {
        // Function from globalfunctions.cpp
        messageEngine("Read only...cannot create folder", MessageType::Warning);
        return;
    }

    // Create new directory
    QModelIndex i = modelList->index(ui->pathEdit->itemText(0));
    newDir = modelView->mapFromSource(modelList->insertFolder(i));
    listSelectionModel->setCurrentIndex(newDir,QItemSelectionModel::ClearAndSelect);

    // Editation of name of new directory
    if (ui->view->currentIndex() == 0) ui->viewIcon->edit(newDir);
    else ui->viewDetail->edit(newDir);
}

void corefm::on_actionNewTextFile_triggered()
{
    // Check whether current directory is writeable
    QModelIndex fileIndex;
    if (!QFileInfo(ui->pathEdit->itemText(0)).isWritable()) {
        // Function from globalfunctions.cpp
        messageEngine("Read only...cannot create file", MessageType::Warning);
        return;
    }

    // Create new file
    QModelIndex i = modelList->index(ui->pathEdit->itemText(0));
    fileIndex = modelView->mapFromSource(modelList->insertFile(i));
    listSelectionModel->setCurrentIndex(fileIndex,QItemSelectionModel::ClearAndSelect);

     // Editation of name of new file
    if (ui->view->currentIndex() == 0) ui->viewIcon->edit(fileIndex);
    else ui->viewDetail->edit(fileIndex);
}

void corefm::on_actionNewPage_triggered()
{
    const QString path(curIndex.filePath());
    if(curIndex.isDir())
        addTab(path);
}

void corefm::on_SHome_clicked()
{
    QModelIndex i = modelTree->mapFromSource(modelList->index(QDir::homePath()));
    ui->viewDir->setCurrentIndex(i);
}

void corefm::on_actionTerminal_triggered()
{
    QString defultTerminal = sm.getTerminal(); // selected terminal name from settings.
    QStringList args(defultTerminal.split(" "));
    QString name = args.at(0);
    args.removeAt(0);

    const QString path(curIndex.filePath());

    if (name == "CoreTerminal") {
        appEngine(CoreTerminal,path);
    } else {
        QProcess::startDetached(name, args, path);
    }

    // Function from globalfunctions.cpp
    QString mess = defultTerminal + " opening " ;
    messageEngine(mess, MessageType::Info);
}

void corefm::setSortColumn(QAction *columnAct)
{
    // Set root index
    if (ui->viewIcon->rootIndex() != modelList->index(ui->pathEdit->currentText())) {
      QModelIndex i = modelList->index(ui->pathEdit->currentText());
      ui->viewIcon->setRootIndex(modelView->mapFromSource(i));
    }

    columnAct->setChecked(true);

    if (columnAct == ui->actionName) {
      currentSortColumn =  0;
    }
    else if (columnAct == ui->actionDate) {
      currentSortColumn =  3;
    }
    else if (columnAct == ui->actionSize) {
      currentSortColumn = 1;
    }
}

void corefm::toggleSortBy(QAction *action) //Sets sort column
{
    setSortColumn(action);
    modelView->sort(currentSortColumn, currentSortOrder);
}

void corefm::on_actionAscending_triggered(bool checked) //Sets sort order
{
    if (ui->viewIcon->rootIndex() != modelList->index(ui->pathEdit->currentText())) {
      QModelIndex i = modelList->index(ui->pathEdit->currentText());
      ui->viewIcon->setRootIndex(modelView->mapFromSource(i));
    }

    if(checked){
        currentSortOrder = Qt::AscendingOrder;
        modelView->sort(currentSortColumn, Qt::AscendingOrder);
    } else{
        currentSortOrder = Qt::DescendingOrder;
        modelView->sort(currentSortColumn, Qt::DescendingOrder);
    }
}

/**
 * @brief Executes a file
 * @param index
 * @param run
 */
void corefm::executeFile(QModelIndex index, bool run)
{
    // Index of file
    QModelIndex srcIndex = modelView->mapToSource(index);

    // Run or open
    if (run == true) {
        const QString path = modelList->filePath(srcIndex);
        QProcess::startDetached("xdg-open", QStringList() << path);
    } else if (run == false) {
        mimeUtils->openInApp(modelList->fileInfo(srcIndex), this);
    }
}

void corefm::goTo(const QString path)
{
    if (!path.isEmpty()){
        QModelIndex i = modelTree->mapFromSource(modelList->index(path));
        ui->viewDir->setCurrentIndex(i);
        on_actionRefresh_triggered();
    } else  {
        QModelIndex i = modelTree->mapFromSource(modelList->index(startPath));
        ui->viewDir->setCurrentIndex(i);
        on_actionRefresh_triggered();
    }
}

void corefm::on_SDesktop_clicked()
{
    QModelIndex i = modelTree->mapFromSource(modelList->index(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)));
    ui->viewDir->setCurrentIndex(i);
}

void corefm::on_SDownloads_clicked()
{
    QModelIndex i = modelTree->mapFromSource(modelList->index(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)));
    ui->viewDir->setCurrentIndex(i);
    on_actionRefresh_triggered();
}

void corefm::on_actionSelectAll_triggered()
{
    if(ui->view->currentIndex() == 0){
        ui->viewIcon->selectAll();
    } else {
        ui->viewDetail->selectAll();
    }
}

void corefm::on_Tools_clicked(bool checked)
{
    if(checked){
        ui->toolsBar->show();
    } else{
        ui->toolsBar->hide();
    }
}

void corefm::on_actionCorePlayer_triggered()
{
    const QString path(curIndex.filePath());
    appEngine(CorePlayer, path);
}

void corefm::on_actionCorePad_triggered()
{
    //fix
    const QString path(curIndex.filePath());
    appEngine(CorePad, path);
}

void corefm::on_actionCoreFM_triggered()
{
    appEngine(CoreFM, QFileInfo(curIndex.filePath()).path());
}

void corefm::on_actionCoreImage_triggered()
{
    const QString path(curIndex.filePath());
    appEngine(CoreImage, path);
}

void corefm::on_actionCorePaint_triggered()
{
    const QString path(curIndex.filePath());
    appEngine(CorePaint, path);
}

void corefm::on_actionCorePDF_triggered()
{
    const QString path(curIndex.filePath());
    appEngine(CorePDF, path);
}


void corefm::on_actionTrash_it_triggered()
{
    if (selectItemCount != 0) {
        // Function from globalfunctions.cpp
        moveToTrash(curIndex.filePath());
        on_actionRefresh_triggered();
    }
}

void corefm::on_showHidden_clicked(bool checked)
{
    if(!checked){
        if (curIndex.isHidden()) {
          listSelectionModel->clear();
        }
        modelView->setFilterRegExp("no");
        modelTree->setFilterRegExp("no");
    } else if (checked){
        modelView->setFilterRegExp("");
        modelTree->setFilterRegExp("");
    }
    modelView->invalidate();
    dirLoaded();
}

void corefm::on_SBookMarkIt_clicked()
{
    const QString path(curIndex.filePath());
    bookmarks bookMarks;
    bookMarks.callBookMarkDialog(this,path);
}

void corefm::on_searchHere_clicked()
{
    const QString folderPath(ui->pathEdit->itemText(0));
    appEngine(Search, folderPath);
}

void corefm::on_actionExtract_Here_triggered()
{
    const QString path(curIndex.filePath());
    const QString folderPath(ui->pathEdit->itemText(0));
    corearchiver *arc = new corearchiver();
    arc->extract(path, QDir(folderPath));
}

void corefm::on_actionCreate_Archive_triggered()
{
    const QString path(curIndex.filePath());
    const QString folderPath(ui->pathEdit->itemText(0));
    corearchiver *arc = new corearchiver();
    arc->setFilename(curIndex.fileName());
    arc->setFolderPath(folderPath);
    arc->filePathList = QStringList() << path;
    arc->show();
}

void corefm::on_STrash_clicked()
{
    QModelIndex i = modelTree->mapFromSource(modelList->index(QDir::homePath() + "/.local/share/Trash/files"));
    ui->viewDir->setCurrentIndex(i);
    on_actionRefresh_triggered();
    ui->emptyTrash->setVisible(1);
}

void corefm::on_emptyTrash_clicked()
{
    on_actionSelectAll_triggered();
    on_actionDelete_triggered();
}

void corefm::blockDevicesChanged()
{
    ui->partitions->clear();

    //Add detected block devices
    QProcess *lsblk = new QProcess(this);
    lsblk->start("lsblk -rf --output name,label,hotplug,parttype");

    lsblk->waitForFinished();
    QByteArray output = lsblk->readAllStandardOutput();

    for (QString block : udisks->blockDevices()) { //Iterate over all detected block devices
        UDisks2Block *device = udisks->blockDevice(block);
        QListWidgetItem *item;
        QIcon icon;
        if (device) { //Check that device actually exists
            if (device->fileSystem()) { //Check that filesystem exists on block device
                if (device->type != "swap") { //Ignore swap devices
                    for (QString part : QString(output).split("\n")) {
                        if (part != "") {
                            QStringList parse = part.split(" ");
                            if (parse.length() > 1) {
                                if (parse[0] == device->fileSystem()->name) {

                                    if (parse[1] == "") {
                                        // Function from globalfunctions.cpp
                                        item = new QListWidgetItem("Drive (" +formatSize(device->size)+ ")");
                                        icon = QIcon(":/icons/drive.svg");
                                    } else {
                                        if (parse.count() > 2) {
                                            if (parse[2] == "0") {
                                                icon = QIcon(":/icons/drive.svg");
                                            } else {
                                                icon = QIcon(":/icons/drive_usb.svg");
                                            }
                                        }
                                        QString itemText(parse[1].replace("\\x20", " "));  // + " (" + device->fileSystem()->name + ")"
                                        item = new QListWidgetItem(itemText);
                                    }
                                }
                            }
                        }
                    }

                    if (!(item)) {
                        // Function from globalfunctions.cpp
                        item = new QListWidgetItem(formatSize(device->size) + " Hard Drive (" + device->fileSystem()->name + ")");
                        icon = QIcon(":/icons/drive.svg");
                    }

                    if (device->fileSystem()->mountPoints().count() == 0) {
                        QPainter *p = new QPainter();
                        QPixmap temp = icon.pixmap(22,22);
                        p->begin(&temp);
                        p->drawPixmap(10,10,10,10,QIcon(":/icons/emblem-unmounted.svg").pixmap(8,8));
                        p->end();
                        icon = QIcon(temp);
                    } else {
                        QPainter *p = new QPainter();
                        QPixmap temp = icon.pixmap(22,22);
                        p->begin(&temp);
                        p->drawPixmap(10,10,10,10,QIcon(":/icons/emblem-mounted.svg").pixmap(8,8));
                        p->end();
                        icon = QIcon(temp);
                    }

                    item->setIcon(icon);
                    item->setData(Qt::UserRole, device->fileSystem()->name);
                    ui->partitions->addItem(item);
                }
            }
        }
    }

    //Add detected MTP devices
    if (QFile("/usr/bin/jmtpfs").exists()) {
        //Detect MTP devices
        QProcess* mtpDev = new QProcess(this);
        mtpDev->start("jmtpfs -l");
        mtpDev->waitForStarted();

        while (mtpDev->state() == QProcess::Running) {
            QApplication::processEvents();
        }
        QString output(mtpDev->readAll());
        bool startReading = false;
        for (QString line : output.split("\n")) {
            if (line != "") {
                if (startReading) {
                    QStringList parse = line.split(", "); //busLocation, devNum, productId, vendorId, product, vendor
                    QListWidgetItem* item = new QListWidgetItem();
                    QString text = parse.at(5) + " " + parse.at(4);
                    if (!text.contains("(MTP)")) {
                        text += " (MTP)";
                    }
                    item->setText(parse.at(5) + " " + parse.at(4));
                    item->setIcon(QIcon(":/icons/drive_media.svg"));
                    item->setData(Qt::UserRole, "mtp");
                    item->setData(Qt::UserRole + 1, parse.at(0));
                    item->setData(Qt::UserRole + 2, parse.at(1));
                    ui->partitions->addItem(item);
                } else {
                    if (line.startsWith("Available devices")) {
                        startReading = true;
                    }
                }
            }
        }

    }

    //Detect iOS Devices
    if (QFile("/usr/bin/ifuse").exists() && QFile("/usr/bin/idevicepair").exists() && QFile("/usr/bin/idevice_id").exists()) {
        QProcess* iosDev = new QProcess();
        iosDev->start("idevice_id -l");
        iosDev->waitForStarted();

        while (iosDev->state() == QProcess::Running) {
            QApplication::processEvents();
        }

        QString output(iosDev->readAll());
        for (QString line : output.split("\n")) {
            if (line != "") {
                if (!line.startsWith("ERROR:")) {
                    QListWidgetItem* item = new QListWidgetItem();
                    QProcess* iosName = new QProcess();
                    iosName->start("idevice_id " + line);
                    iosName->waitForFinished();

                    QString name(iosName->readAll());
                    name = name.trimmed();

                    if (name == "") {
                        item->setText("iOS Device");
                    } else {
                        item->setText(name + " (iOS)");
                    }

                    item->setIcon(QIcon(":/icons/drive_media.svg"));
                    item->setData(Qt::UserRole, "ios");
                    item->setData(Qt::UserRole + 1, line);
                    ui->partitions->addItem(item);
                }
            }
        }
    }
}

void corefm::on_partitions_itemClicked(QListWidgetItem *item)
{
    //This is a block or MTP device; mount and navigate to device.
    QString dev = item->data(Qt::UserRole).toString();
    if (dev == "mtp") {
//        qDebug() << "Mounting MTP device " + item->data(Qt::UserRole + 1).toString() + ", " + item->data(Qt::UserRole + 2).toString();

        QString mtpDirName = "mtp" + item->data(Qt::UserRole + 1).toString() + "," + item->data(Qt::UserRole + 2).toString();
//        QDir::home().mkdir(".coreBox");
        QDir(QDir::homePath() + "/.coreBox").mkdir(mtpDirName);
        QProcess *mountProcess = new QProcess(this);

        bool mounted = false;
        for (QString file : QDir(QDir::homePath() + "/.coreBox/" + mtpDirName).entryList()) {
            if (file != "." && file != "..") {
                mounted = true;
            }
        }
        if (!mounted) {
            mountProcess->start("jmtpfs " + QDir::homePath() + "/.coreBox/" + mtpDirName + " -device=" + item->data(Qt::UserRole + 1).toString() + "," + item->data(Qt::UserRole + 2).toString());
            mountProcess->waitForStarted();

            while (mountProcess->state() == QProcess::Running) {
                QApplication::processEvents();
            }
        }
        goTo(QDir::homePath() + "/.coreBox/" + mtpDirName);

    }
    else if (dev == "ios") { //iOS Device
        QString id = item->data(Qt::UserRole + 1).toString();
//        qDebug() << "Mounting iOS Device " + id;

        QProcess* pairProcess = new QProcess(this);
        pairProcess->start("idevicepair -u " + id + " pair");
        pairProcess->waitForStarted();

        while (pairProcess->state() == QProcess::Running) {
            QApplication::processEvents();
        }

        QString pairOutput(pairProcess->readAll());
        if (pairOutput.startsWith("ERROR:")) {
            if (pairOutput.contains("Please enter the passcode")) { // Ask user to unlock device
                QMessageBox::critical(this, "iOS Device Locked", "The device is locked. Enter the passcode on the device and try again.", QMessageBox::Ok, QMessageBox::Ok);
            } else if (pairOutput.contains("Please accept the trust dialog")) { //Ask user to trust PC
                QMessageBox::critical(this, "iOS Device Trust", "Your device does not trust this PC. To access the device, you need to trust this PC. Answer the trust dialog on your device and try again.", QMessageBox::Ok, QMessageBox::Ok);
            } else if (pairOutput.contains("user denied the trust dialog")) { //User did not trust PC
                QMessageBox::critical(this, "iOS Device Trust", "We can't access this device because you told it to not trust this computer.", QMessageBox::Ok, QMessageBox::Ok);
            } else { //Generic Error
                QMessageBox::critical(this, "iOS Error", "An error occurred trying to pair with the device:\n\n" + pairOutput, QMessageBox::Ok, QMessageBox::Ok);
            }
            return;
        }
        else {
            QString iosDirName = "ios" + id;
//            QDir::home().mkdir(".coreBox");
            QDir(QDir::homePath() + "/.coreBox").mkdir(iosDirName);

            QProcess* mountProcess = new QProcess();

            bool mounted = false;
            for (QString file : QDir(QDir::homePath() + "/.coreBox/" + iosDirName).entryList()) {
                if (file != "." && file != "..") {
                    mounted = true;
                }
            }
            if (!mounted) {
                mountProcess->start("ifuse -o ro " + QDir::homePath() + "/.coreBox/" + iosDirName + " -u " + id);
                mountProcess->waitForStarted();

                while (mountProcess->state() == QProcess::Running) {
                    QApplication::processEvents();
                }
            }
        }

    }
    else {
//        qDebug() << "Mounting " + dev;
        if (udisks->blockDevice(dev)->fileSystem()->mountPoints().count() == 0) {
            QString mountpoint = udisks->blockDevice(dev)->fileSystem()->mount();
            while (udisks->blockDevice(dev)->fileSystem()->mountPoints().count() == 0) {
                QApplication::processEvents();
            }
            if (mountpoint == "") {
                // Function from globalfunctions.cpp
                messageEngine("Couldn't mount " + udisks->blockDevice(dev)->dev, MessageType::Warning);
            }
            else {
                goTo(mountpoint);
            }
        }
        else {
            goTo(udisks->blockDevice(dev)->fileSystem()->mountPoints().at(0));
        }
    }
    blockDevicesChanged();
}

void corefm::on_actionDesktop_triggered()
{
    on_actionCopy_triggered();

    const QString newPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    QStringList cutList;

    pasteLauncher(QApplication::clipboard()->mimeData(), newPath, cutList);
    ui->paste->setVisible(false);
    on_actionRefresh_triggered();

    // Function from globalfunctions.cpp
    messageEngine("Send Completed.", MessageType::Info);
}

void corefm::on_actionHome_triggered()
{
    on_actionCopy_triggered();

    const QString newPath( QDir::home().path()) ;
    QStringList cutList;

    pasteLauncher(QApplication::clipboard()->mimeData(), newPath, cutList);
    ui->paste->setVisible(false);
    on_actionRefresh_triggered();

    // Function from globalfunctions.cpp
    messageEngine("send Completed.", MessageType::Info);
}

void corefm::pressed()
{
    ui->viewIcon->setCurrentIndex(QModelIndex());
    ui->viewDetail->setCurrentIndex(QModelIndex());

    ui->name->setText("");
    ui->size->setText("");
}

void corefm::on_actionRun_triggered()
{
    executeFile(listSelectionModel->currentIndex(), true);
}

void corefm::sendToPath()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if(action){
        on_actionCopy_triggered();

        QString newPath = FileUtils::getMountPathByName(action->data().toString()) ;
        QStringList cutList;

        pasteLauncher(QApplication::clipboard()->mimeData(), newPath, cutList);
        ui->paste->setVisible(false);
        on_actionRefresh_triggered();

        // Function from globalfunctions.cpp
        messageEngine("send Completed.", MessageType::Info);
    }
}

void corefm::on_action_Rename_triggered()
{
//    QModelIndexList selList;
//    if(listSelectionModel->selectedRows(0).count()) selList = listSelectionModel->selectedRows(0);
//    else selList = listSelectionModel->selectedIndexes();

//    if(selList.count() == 0) selList << modelView->mapFromSource(modelList->index(ui->pathEdit->currentText()));

//    QStringList paths;

//    foreach(QModelIndex item, selList)
//        paths.append(modelList->filePath(modelView->mapToSource(item)));

//    qDebug()<< "paths"<<paths;
//    qDebug()<< "modelList"<< modelList;

//    qDebug()<< curIndex.filePath();

//    CoreBox *cBox = qobject_cast<CoreBox*>(qApp->activeWindow());
//    cBox->tabEngine(CoreRenamer, curIndex.filePath());
}

void corefm::on_actionCoreRenamer_triggered()
{
    qDebug() << "1";
    const QString path(curIndex.filePath());
    appEngine(CoreRenamer, path);
}

QString corefm::gCurrentPath(int index) {
    tabs->setCurrentIndex(index);
    return ui->pathEdit->currentText();
}

int corefm::tabsCount()
{
    int count = 1;
    if (tabs->count())
        count = tabs->count();

    return count;
}

void corefm::on_viewIcon_customContextMenuRequested(const QPoint &pos)
{
    globalmenu()->exec(ui->viewIcon->mapToGlobal(pos));
}

void corefm::on_viewDetail_customContextMenuRequested(const QPoint &pos)
{
    globalmenu()->exec(ui->viewDetail->mapToGlobal(pos));
}

void corefm::on_showthumb_clicked(bool checked)
{
    modelList->setMode(checked);
    on_actionRefresh_triggered();
}

void corefm::on_actionItemsToText_triggered()
{
    FileUtils::getFileFolderTree(curIndex.filePath());
}


void corefm::viewMode(bool mode)
{
    if(mode){
        // Set root index
        if (ui->viewIcon->rootIndex() != modelList->index(ui->pathEdit->currentText())) {
          QModelIndex i = modelList->index(ui->pathEdit->currentText());
          ui->viewIcon->setRootIndex(modelView->mapFromSource(i));
        }

        currentView = 1;

        ui->viewIcon->setViewMode(QListView::IconMode);
        ui->viewIcon->setGridSize(QSize(zoom + 32, zoom + 32));
        ui->viewIcon->setIconSize(QSize(zoom, zoom));
        modelList->setMode(ui->showthumb->isChecked());
        ui->view->setCurrentIndex(0);

        ui->viewDetail->setMouseTracking(false);
        ui->viewIcon->setMouseTracking(true);

        if (tabs->count()) tabs->setType(1);
    }

    else {
        // set root index
        QModelIndex i = modelList->index(ui->pathEdit->currentText());
        if (ui->viewDetail->rootIndex() != i) {
          ui->viewDetail->setRootIndex(modelView->mapFromSource(i));
        }

        currentView = 2;

        modelList->setMode(ui->showthumb->isChecked());
        ui->view->setCurrentIndex(1);
        ui->viewDetail->setMouseTracking(true);
        ui->viewIcon->setMouseTracking(false);

        if (tabs->count()) tabs->setType(2);
    }
}


void corefm::on_viewMode_clicked()
{
    if(ui->view->currentIndex() == 1){
        viewMode(true);
    }else{
        viewMode(false);
    }
}
