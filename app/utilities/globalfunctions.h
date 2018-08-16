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

#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QApplication>

#include <QFile>
#include <QDir>


#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>

#include <QDirIterator>
#include <QProcess>



#include <QTextStream>

#include "settings/settingsmanage.h"






enum AppsName {
    damn = -1,
    CoreFM = 0,
    CoreImage,
    CorePad,
    CorePaint,
    CorePlayer,
    Dashboard,
    Bookmarks,
    About,
    StartView,
    Help,
    Settings,
    Search,
    CoreTime,
    CoreRenamer,
    CorePDF,
    CoreTerminal
};


enum Category {
    FileManager,
    TextEditor,
    ImageViewer,
    Terminal
};



void appEngine(Category ctg , const QString path);

  void appEngines(AppsName i, const QString arg);


// bool checkRecentActivityFile();


 void appSelectionEngine(const QString &path);









#endif // GLOBALFUNCTIONS_H
