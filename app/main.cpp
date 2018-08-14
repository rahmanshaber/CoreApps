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

#include "coreaction/coreaction.h"
#include "start/start.h"
#include "corefm/corefm.h"

#include "utilities/utilities.h"
#include "settings/settingsmanage.h"

#include <QApplication>
#include <QFont>
#include <QStyleFactory>


void startSetup()
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // set all the requried folders
    setupFolder(FolderSetup::BookmarkFolder);
    setupFolder(FolderSetup::DriveMountFolder);
    setupFolder(FolderSetup::TrashFolder);

    // if setting file not exist create one with defult
    SettingsManage sm;
    sm.createDefaultSettings();

    // set a icon across all the apps
    QIcon::setThemeName(sm.getThemeName());

    // set one font style across all the apps
    QFont fl (sm.getFontStyle(), 10, QFont::Normal);
    QApplication::setFont(fl);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setQuitOnLastWindowClosed(true);

    startSetup();

    // Set application info
    app.setOrganizationName("o");
    app.setApplicationName("o");
    app.setWindowIcon(QIcon(":/app/icons/app-icons/CoreAction.svg"));

    Start s;
    s.show();

    corefm a;
    a.show();

    coreaction e;
    e.show();

    return app.exec();
}

