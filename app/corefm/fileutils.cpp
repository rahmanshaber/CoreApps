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

#include "fileutils.h"


/**
 * @brief Recursive removes file or directory
 * @param path path to file
 * @param name name of file
 * @return true if file/directory was successfully removed
 */
bool FileUtils::removeRecurse(const QString &path, const QString &name)
{
    // File location
    QString url = path + QDir::separator() + name;

    // Check whether file or directory exists
    QFileInfo file(url);
    if (!file.exists()) {
      return false;
    }

    // List of files that will be deleted
    QStringList files;

    // If given file is a directory, collect all children of given directory
    if (file.isDir()) {
      QDirIterator it(url, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot
                      | QDir::Hidden, QDirIterator::Subdirectories);
      while (it.hasNext()) {
        files.prepend(it.next());
      }
    }

    // Append given file to the list of files and delete all
    files.append(url);
    foreach (QString file, files) {
      QFile(file).remove();
    }
    return true;
}

/**
 * @brief Collects all file names in given path (recursive)
 * @param path path
 * @param parent parent path
 * @param list resulting list of files
 */
void FileUtils::recurseFolder(const QString &path, const QString &parent,QStringList *list)
{
    // Get all files in this path
    QDir dir(path);
    QStringList files = dir.entryList(QDir::AllEntries | QDir::Files
                                      | QDir::NoDotAndDotDot | QDir::Hidden);

    // Go through all files in current directory
    for (int i = 0; i < files.count(); i++) {

      // If current file is folder perform this method again. Otherwise add file
      // to list of results
      QString current = parent + QDir::separator() + files.at(i);
      if (QFileInfo(files.at(i)).isDir()) {
        recurseFolder(files.at(i), current, list);
      }
      else list->append(current);
    }
}

/**
 * @brief Returns size of all given files/dirs (including nested files/dirs)
 * @param files
 * @return total size
 */
qint64 FileUtils::totalSize(const QList<QUrl> &files)
{
    qint64 total = 1;
    foreach (QUrl url, files) {
      QFileInfo file = url.path();
      if (file.isFile()) total += file.size();
      else {
        QDirIterator it(url.path(), QDir::AllEntries | QDir::System
                        | QDir::NoDotAndDotDot | QDir::NoSymLinks
                        | QDir::Hidden, QDirIterator::Subdirectories);
        while (it.hasNext()) {
          it.next();
          total += it.fileInfo().size();
        }
      }
    }
    return total;
}

/**
 * @brief Returns names of available applications
 * @return application name list
 */
QStringList FileUtils::getApplicationNames()
{
    QStringList appNames;
    QDirIterator it("/usr/share/applications", QStringList("*.desktop"),
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
      it.next();
      appNames.append(it.fileName());
    }
    return appNames;
}

/**
 * @brief Returns list of available applications
 * @return application list
 */
QList<DesktopFile> FileUtils::getApplications()
{
    QList<DesktopFile> apps;
    QDirIterator it("/usr/share/applications", QStringList("*.desktop"),
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
      it.next();
      apps.append(DesktopFile(it.filePath()));
    }
    return apps;
}

/**
 * @brief Returns real suffix for given file
 * @param name
 * @return suffix
 */
QString FileUtils::getRealSuffix(const QString &name)
{
    // Strip version suffix
    QStringList tmp = name.split(".");
    bool ok;
    while (tmp.size() > 1) {
      tmp.last().toInt(&ok);
      if (!ok) {
        return tmp.last();
      }
      tmp.removeLast();
    }
    return "";
}

/**
 * @brief Returns mime icon
 * @param mime
 * @return icon
 */
QIcon FileUtils::searchMimeIcon(QString mime, const QIcon &defaultIcon)
{
    QIcon icon = QIcon::fromTheme(mime.replace("/", "-"));
    return icon.isNull() ? defaultIcon : icon;
}

/**
 * @brief Searches for generic icon
 * @param category
 * @return icon
 */
QIcon FileUtils::searchGenericIcon(const QString &category,const QIcon &defaultIcon)
{
    QIcon icon = QIcon::fromTheme(category + "-generic");
    if (!icon.isNull()) {
      return icon;
    }
    icon = QIcon::fromTheme(category + "-x-generic");
    return icon.isNull() ? defaultIcon : icon;
}

/**
 * @brief Searches for application icon in the filesystem
 * @param app
 * @param defaultIcon
 * @return icon
 */
QIcon FileUtils::searchAppIcon(const DesktopFile &app,const QIcon &defaultIcon)
{
    // Resulting icon
    QIcon icon;

    // First attempt, check whether icon is a valid file
    if (QFile(app.getIcon()).exists()) {
      icon = QIcon(app.getIcon());
      if (!icon.isNull()) {
        return icon;
      }
    }

    // Second attempt, try load icon from theme
    icon = QIcon::fromTheme(app.getIcon());
    if (!icon.isNull()) {
      return icon;
    }

    // Next, try luck with application name
    QString name = app.getFileName().remove(".desktop").split("/").last();
    icon = QIcon::fromTheme(name);
    if (!icon.isNull()) {
      return icon;
    }

    // Last chance
    QDir appIcons("/usr/share/pixmaps","", nullptr, QDir::Files | QDir::NoDotAndDotDot);
    QStringList iconFiles = appIcons.entryList();
    QStringList searchIcons = iconFiles.filter(name);
    if (searchIcons.count() > 0) {
      return QIcon("/usr/share/pixmaps/" + searchIcons.at(0));
    }

    // Default icon
    return defaultIcon;
}

QString FileUtils::getFileFolderTree(const QString &path)
{
    int pat = 0;
    QString strDir;


    // ├

    // └

    // │

    // ──


    pat++;

    QDir dir(path);
    //dir.setSorting(QDir::DirsFirst);
    //dir.setNameFilters(QStringList("*"));
    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList dList = dir.entryList();
    for (int i = 0; i < dList.count(); ++i) {
        QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dList.at(i));

        QString strPat = "├── ";
        for (int i = 1; i < pat; i++) {
            strPat.insert(0, "│   ");
        }

        if (i == dList.count() - 1) {
            int pos = strPat.count() - 4;
            strPat = strPat.replace(pos, 1, "└");
        }

        strDir.append(strPat + QDir(dList.at(i)).dirName() + "\n");

        getFileFolderTree(newPath);
    }

    pat--;

    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList fList = dir.entryList();
    for (int i = 0; i < fList.count(); i++) {

        QString strPat = "├── ";
        for (int j = 1; j < pat + 1; j++) {
            strPat.insert(0, "│   ");
        }

        if (i == fList.count() - 1) {
            strPat = strPat.replace(strPat.count() - 4, 1, "└");
        }

        strDir.append(strPat + fList[i] + "\n");
    }

    if (!strDir.count())
        strDir.append(".\n");


    // Save path for the collected file folder list
    QString filePath = path + "/" + QFileInfo(path).baseName() + ".txt";

    QFile file(filePath);
    file.open(QIODevice::Text | QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream out(&file);
    out << strDir;
    file.close();


    return QString();
}

QString FileUtils::getMountPathByName(const QString displayName) // get mount path by partition display name
{
    if(displayName.isNull() || displayName.isEmpty()) return "";

    else {
        const auto allMounted = QStorageInfo::mountedVolumes();
        for(auto& singleMounted : allMounted){
            if (singleMounted.displayName() == displayName) return singleMounted.rootPath();
        }
    }
    return "";
}

QString FileUtils::getFileSize(const QString &path) //get size of single file in int
{
    return getF(QStringList() << path).sizeText;
}

QString FileUtils::getMultipleFileSize(const QStringList &paths) // get file size of multiple files
{
    return getF(paths).sizeText;
}

QString FileUtils::getMultipleCountText(const QStringList &paths)
{
    return getF(paths).countText;
}

QString FileUtils::getCountText(const QString path) //get size of single file in string
{
    return getF(QStringList() << path).countText;
}

SizeAndCountText FileUtils::getF(const QStringList &paths)
{
    qint64 totalSize = 0;
    int files = 0;
    int folders = 0;
    Q_FOREACH( QString path, paths ) {
        if ( QFileInfo( path ).isDir() ) {
            QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories );
            while ( it.hasNext() ) {
                it.next();
                if( it.fileInfo().isDir() ) {
                    if ( it.filePath() == path )
                        continue;

                    folders++;
                } else {
                    files++;
                    totalSize += it.fileInfo().size();
                }
            }
        } else {
            files++;
            totalSize += getSize( path );
        }
    }

    SizeAndCountText sc;
    sc.countText = QString("%1 Files, %2 Folders").arg(files).arg(folders);
    sc.sizeText = formatSize(totalSize);
    return sc;
}

qint64 FileUtils::getSize(const QString &path)
{

    struct stat statbuf;
    if ( stat( path.toLocal8Bit().constData(), &statbuf ) != 0 )
        return 0;

    switch( statbuf.st_mode & S_IFMT ) {
        case S_IFREG: {

            return statbuf.st_size;
        }

        case S_IFDIR: {
            DIR* d_fh;
            struct dirent* entry;
            QString longest_name;

            while ( ( d_fh = opendir( path.toLocal8Bit().constData() ) ) == nullptr ) {
                qWarning() << "Couldn't open directory:" << path;
                return statbuf.st_size;
            }

            quint64 size = statbuf.st_size;

            longest_name = QString( path );
            if ( not longest_name.endsWith( "/" ) )
                longest_name += "/";

            while( ( entry = readdir( d_fh ) ) != nullptr ) {

                /* Don't descend up the tree or include the current directory */
                if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

                    if ( entry->d_type == DT_DIR ) {

                        /* Recurse into that folder */
                        size += getSize( longest_name + entry->d_name );
                    }

                    else {

                        /* Get the size of the current file */
                        size += getSize( longest_name + entry->d_name );
                    }
                }
            }

            closedir( d_fh );
            return size;
        }

        default: {

            /* Return 0 for all other nodes: chr, blk, lnk, symlink etc */
            return 0;
        }
    }
}
