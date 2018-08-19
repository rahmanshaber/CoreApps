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
#include "corefm/corefm.h"

#include "coreimage/coreimage.h"
#include "corepad/corepad.h"
#include "corepaint/corepaint.h"
#include "corepdf/corepdf.h"
#include "coreplayer/coreplayer.h"
#include "corerenamer/corerenamer.h"
#include "coreshot/coreshotdialog.h"
#include "coreterminal/coreterminal.h"
#include "coretime/coretime.h"
#include "dashboard/dashboard.h"
#include "help/help.h"
#include "search/search.h"
#include "settings/settings.h"
#include "start/start.h"
#include "coreinfo/coreinfo.h"

#include "utilities/utilities.h"
#include "settings/settingsmanage.h"

#include <QApplication>
#include <QFont>
#include <QStyleFactory>

#include <QProcess>
#include <QStandardPaths>
#include <QDir>


void printUsage( const char *exec ) {
    qDebug() << "CoreBox v2.2\n"
             << "Usage:\n\tcoreBox [options] filePath\n"
             << "Example:\n\tcoreBox --corepad ./example.txt"
             << "\nOptions:\n"
             << "\t--about                             Show info about this application.\n"
             << "\t--bookmarks                         Bookmark files\n"
             << "\t--coreaction                        Show CoreAction\n"
             << "\t--corearchiver -e ZipFilePath       Extract Archive\n"
             << "\t--corearchiver -a FilePath(s)       Archive Files\n"
             << "\t--corefm FilePath(s)                Open File Manager\n"
             << "\t--coreimage FilePath(s)             Open image viewer\n"
             << "\t--corepad FilePath(s)               Open text viewer\n"
             << "\t--corepaint FilePath(s)             Open paint\n"
             << "\t--corepdf FilePath(s)               Open PDF viewer\n"
             << "\t--coreplayer FilePath(s)            Open media files\n"
             << "\t--corerenamer FilePath(s)           Rename files\n"
             << "\t--coreshot                          Take screenshot\n"
             << "\t--coreterminal Argument(s)          Open terminal\n"
             << "\t--coretime                          Show date time\n"
             << "\t--dashboard                         Show system information\n"
             << "\t--help                              Show help\n"
             << "\t--search SearchText SearchPath      Search files in a path\n"
             << "\t--settings                          Show settings\n"
             << "\t--start                             Show start\n"
             << "\t--coreinfo FilePath(s)              Show info about a file";
    qDebug() << "Problem occurs at " << exec;
}

void startSetup()
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // set all the requried folders
    setupFolder(Utilities::FolderSetup::BookmarkFolder);
    setupFolder(Utilities::FolderSetup::DriveMountFolder);
    setupFolder(Utilities::FolderSetup::TrashFolder);

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

//    Start s;
//    s.show();


    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QList< QCommandLineOption > options;
    const int winCount = 19;

    options << QCommandLineOption( QStringList( ) << "about", "Open about window" )
            << QCommandLineOption( QStringList( ) << "bookmarks", "Bookmark files" )
            << QCommandLineOption( QStringList( ) << "coreaction", "Open popup action window" )
            << QCommandLineOption( QStringList( ) << "corearchiver -a", "Archive files" )
            << QCommandLineOption( QStringList( ) << "corearchiver -e", "Extract Archive" )
            << QCommandLineOption( QStringList( ) << "corefm", "Open file manager" )
            << QCommandLineOption( QStringList( ) << "coreimage", "Open image viewer" )
            << QCommandLineOption( QStringList( ) << "corepad", "Open text editor" )
            << QCommandLineOption( QStringList( ) << "corepaint", "Open paint" )
            << QCommandLineOption( QStringList( ) << "corepdf", "Open PDF viewer" )
            << QCommandLineOption( QStringList( ) << "coreplayer", "Open media files" )
            << QCommandLineOption( QStringList( ) << "corerenamer", "Rename files" )
            << QCommandLineOption( QStringList( ) << "coreshot", "Take screenshot" )
            << QCommandLineOption( QStringList( ) << "coreterminal", "Open terminal" )
            << QCommandLineOption( QStringList( ) << "coretime", "Show time and date" )
            << QCommandLineOption( QStringList( ) << "dashboard", "Show System information" )
            << QCommandLineOption( QStringList( ) << "help", "Show help" )
            << QCommandLineOption( QStringList( ) << "search", "Search files" )
            << QCommandLineOption( QStringList( ) << "settings", "Open settings" )
            << QCommandLineOption( QStringList( ) << "start", "Show start" )
            << QCommandLineOption( QStringList( ) << "coreinfo", "Show information about a file" );

    parser.addOptions(options);

    const QString files = "[FILE1, FILE2,...]";
    parser.addPositionalArgument("files", files, files);

    parser.process(app);

    QStringList args = parser.positionalArguments();

    QStringList paths;
    foreach (QString arg, args) {
      QFileInfo info(arg);
      paths.push_back(info.absoluteFilePath());
    }

    if ( parser.isSet( "about" ) ) {
        about *ab = new about;
        ab->show();

        paths.clear();
        args.clear();
    } else if ( parser.isSet( "bookmarks" ) ) {
        bookmarks *bk = new bookmarks;
        // Create sendFiles( QStringList ) function for sending paths
        bk->sendFiles(paths);
        //bk->show( );
    } else if ( parser.isSet( "coreaction" ) ) {
        coreaction *ca = new coreaction;
        ca->show();

        paths.clear();
        args.clear();
    } else if ( parser.isSet( "corearchiver -a" ) ) {
        corearchiver *carc = new corearchiver;
        // Create sendFiles( QStringList ) function for sending paths
        carc->sendFiles(paths, true);
        carc->show();
    } else if ( parser.isSet( "corearchiver -e" ) ) {
        corearchiver *carc = new corearchiver;
        // Create sendFiles( QStringList ) function for sending paths
        carc->sendFiles(paths, false);
        carc->show();
    } else if ( parser.isSet( "corefm" ) ) {
        corefm *cfm = new corefm;
        // Create sendFiles( QStringList ) function for sending paths
        cfm->sendFiles(paths);
        cfm->show();
    } else if ( parser.isSet( "coreimage" ) ) {
        coreimage *cimg = new coreimage;
        // Create sendFiles( QStringList ) function for sending paths
        cimg->show();
    } else if ( parser.isSet( "corepad" ) ) {
        corepad *cpad = new corepad;
        // Create sendFiles( QStringList ) function for sending paths
        cpad->sendFiles(paths);
        cpad->show();
    } else if ( parser.isSet( "corepaint" ) ) {
        corepaint *cpaint = new corepaint;
        // Create sendFiles( QStringList ) function for sending paths
        cpaint->sendFiles(paths);
        cpaint->show();
    } else if ( parser.isSet( "corepdf" ) ) {
        corepdf *cpdf = new corepdf;
        // Create sendFiles( QStringList ) function for sending paths
        cpdf->sendFiles(paths);
        cpdf->show();
    } else if ( parser.isSet( "coreplayer" ) ) {
        coreplayer *cplay = new coreplayer;
        // Create sendFiles( QStringList ) function for sending paths
        cplay->sendFiles(paths);
        cplay->show();
    } else if ( parser.isSet( "corerenamer" ) ) {
        corerenamer *cren = new corerenamer;
        // Create sendFiles( QStringList ) function for sending paths
        cren->sendFiles(paths);
        cren->show();
    } else if ( parser.isSet( "coreshot" ) ) {
        coreshotdialog *cshot = new coreshotdialog;
        cshot->show();

        args.clear();
        paths.clear();
    } else if ( parser.isSet( "coreterminal" ) ) {
        coreterminal *cterm = new coreterminal;
        // Create sendFiles( QStringList ) function for sending paths
        cterm->show();
    } else if ( parser.isSet( "coretime" ) ) {
        coretime *ctime = new coretime;
        ctime->show();

        args.clear();
        paths.clear();
    } else if ( parser.isSet( "dashboard" ) ) {
        dashboard *cdash = new dashboard;
        cdash->show();

        args.clear();
        paths.clear();
    } else if ( parser.isSet( "help" ) ) {
        help *chelp = new help;
        chelp->show();

        args.clear();
        paths.clear();
    } else if ( parser.isSet( "search" ) ) {
        search *csearch = new search;
        // Create sendFiles( QStringList ) function for sending paths
        csearch->show();
    } else if ( parser.isSet( "settings" ) ) {
        settings *csett = new settings;
        csett->show();

        args.clear();
        paths.clear();
    } else if ( parser.isSet( "start" ) ) {
        Start *cstart = new Start;
        cstart->show();

        args.clear();
        paths.clear();
    } else if ( parser.isSet( "coreinfo" ) ) {
        coreinfo *cinfo = new coreinfo;
        // Create sendFiles( QStringList ) function for sending paths
        cinfo->sendFiles(paths);
        cinfo->show();
    } else {
        printUsage( argv[ 1 ] );
        //return app.exec();

        // For tesing
        Start *cstart = new Start;
        cstart->show();
        args.clear();
        paths.clear();
    }




    // experement

//    QProcess q1;
//    q1.start("xfce4-terminal", QStringList() << "-c" << "htop");


//    QProcess *q1 = new QProcess;;
//    QString k("htop");
//    QString l(QDir::homePath());
//    QStringList args;
//    args << k;
////    q1->setWorkingDirectory(QDir::homePath());
//    q1->startDetached("xfce4-terminal",QStringList() << "htop",l);
//    q1->waitForFinished();
//    qDebug()<< q1->readAllStandardOutput();

//    QProcess *Process = new QProcess();
//        QString exec = "xfce4-terminal";
//        QStringList params;
//        params <<"ls";
//        Process->start(exec,params,QIODevice::ReadWrite);
//        Process->waitForFinished(-1);
//        QString p_stdout = Process->readAllStandardOutput();
//        qDebug()<<p_stdout;

//    corefm a;
//    a.show();

//    coreaction e;
//    e.show();

    return app.exec();
}

