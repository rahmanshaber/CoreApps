/*
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

#include "mimeutils.h"
#include "fileutils.h"

#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>

#include "utilities/utilities.h"
#include "utilities/globalfunctions.h"


/**
 * @brief Creates mime utils
 * @param parent
 */
MimeUtils::MimeUtils(QObject *parent) : QObject(parent)
{
  defaultsFileName = "/.config/coreBox/mimeapps.list";
  defaults = new Properties();
  loadDefaults();
}

/**
 * @brief Loads list of default applications for mimes
 * @return properties with default applications
 */
void MimeUtils::loadDefaults()
{
  defaults->load(QDir::homePath() + defaultsFileName, "Default Applications");
  defaultsChanged = false;
}

/**
 * @brief Destructor
 */
MimeUtils::~MimeUtils()
{
  delete defaults;
}

/**
 * @brief Returns list of mime types
 * @return list of available mimetypes
 */
QStringList MimeUtils::getMimeTypes() const
{
  // Check whether file with mime descriptions exists
  QFile file("/usr/share/mime/types");
  if (!file.exists()) {
    return QStringList();
  }

  // Try to open file
  if (!file.open(QFile::ReadOnly)) {
    return QStringList();
  }

  // Read mime types
  QStringList result;
  QTextStream stream(&file);
  while (!stream.atEnd()) {
    result.append(stream.readLine());
  }
  file.close();
  return result;
}

/**
 * @brief Opens file in a default application
 * @param file
 * @param processOwner
 */
void MimeUtils::openInApp(const QFileInfo &file, QObject *processOwner)
{
    QMimeDatabase mimetype;
    QString mime = mimetype.mimeTypeForFile(file.filePath()).name();
    QString app = defaults->value(mime).toString().split(";").first();
    if (!app.isEmpty()) {
      DesktopFile df = DesktopFile("/usr/share/applications/" + app);
      openInApp(df.getExec(), file, processOwner);
    } else {
        // Function from globalfunctions.cpp
        messageEngine(tr("No default application for mime:\n %1!").arg(mime),MessageType::Warning);
    }
}

/**
 * @brief Opens file in a given application
 * @param exe name of application to be executed
 * @param file to be opened in executed application
 * @param processOwner process owner (default NULL)
 */
void MimeUtils::openInApp(QString exe, const QFileInfo &file,QObject *processOwner)
{
  // This is not the right the solution, but qpdfview won't start otherwise
  // TODO: Repair it correctly
  if (exe.contains("qpdfview")) {
    exe = "qpdfview";
  }

  // Separate application name from its arguments
  QStringList split = exe.split(" ");
  QString name = split.takeAt(0);
  QString args = split.join(" ");

  // Get relative path
  //args = args.split(QDir::separator()).last();

  // Replace parameters with file name. If there are no parameters simply append
  // file name to the end of argument list
  if (args.toLower().contains("%f")) {
    args.replace("%f", file.filePath(), Qt::CaseInsensitive);
  } else if (args.toLower().contains("%u")) {
    args.replace("%u", file.filePath(), Qt::CaseInsensitive);
  } else {
    args.append(args.isEmpty() ? "" : " ");
    args.append(/*"\"" + */file.filePath()/* + "\""*/);
  }

  // Function from globalfunctions.cpp
  saveToRecent(name,args);

  // Start application
  QProcess *myProcess = new QProcess(processOwner);
  myProcess->startDetached(name, QStringList() << args);
  myProcess->waitForFinished(1000);
  //myProcess->terminate();
}

/**
 * @brief Sets defaults file name (name of file where defaults are stored)
 * @param fileName
 */
void MimeUtils::setDefaultsFileName(const QString &fileName)
{
  this->defaultsFileName = fileName;
  loadDefaults();
}
//---------------------------------------------------------------------------

/**
 * @brief Returns defaults file name
 * @return name of file where defaults are stored
 */
QString MimeUtils::getDefaultsFileName() const
{
  return defaultsFileName;
}

/**
 * @brief Generates default application-mime associations
 */
void MimeUtils::generateDefaults()
{

  // Load list of applications
  QList<DesktopFile> apps = FileUtils::getApplications();
  QStringList names;

  // Find defaults; for each application...
  // ------------------------------------------------------------------------
  foreach (DesktopFile a, apps) {

    // For each mime of current application...
    QStringList mimes = a.getMimeType();
    foreach (QString mime, mimes) {

      // Current app name
      QString name = a.getPureFileName() + ".desktop";
      names.append(name);

      // If current mime is not mentioned in the list of defaults, add it
      // together with current application and continue
      if (!defaults->contains(mime)) {
        defaults->set(mime, name);
        defaultsChanged = true;
        continue;
      }

      // Retrieve list of default applications for current mime, if it does
      // not contain current application, add this application to list
      QStringList appNames = defaults->value(mime).toString().split(";");
      if (!appNames.contains(name)) {
        appNames.append(name);
        defaults->set(mime, appNames.join(";"));
        defaultsChanged = true;
      }
    }
  }

  // Delete dead defaults (non existing apps)
  // ------------------------------------------------------------------------
  foreach (QString key, defaults->getKeys()) {
    QStringList tmpNames1 = defaults->value(key).toString().split(";");
    QStringList tmpNames2 = QStringList();
    foreach (QString name, tmpNames1) {
      if (names.contains(name)) {
        tmpNames2.append(name);
      }
    }
    if (tmpNames1.size() != tmpNames2.size()) {
      defaults->set(key, tmpNames2.join(";"));
      defaultsChanged = true;
    }
  }

  // Save defaults if changed
  saveDefaults();
}

/**
 * @brief Sets default mime association
 * @param mime mime name
 * @param apps list of applications (desktop file names)
 */
void MimeUtils::setDefault(const QString &mime, const QStringList &apps)
{
  QString value = apps.join(";");
  if (value.compare(defaults->value(mime, "").toString()) != 0) {
    defaults->set(mime, value);
    defaultsChanged = true;
  }
}

/**
 * @brief Returns default applications for given mime
 * @param mime
 * @return list of default applications name
 */
QStringList MimeUtils::getDefault(const QString &mime) const
{
  return defaults->value(mime).toString().split(";");
}

/**
 * @brief Saves defaults
 */
void MimeUtils::saveDefaults()
{
  if (defaultsChanged) {
    defaults->save(QDir::homePath() + defaultsFileName, "Default Applications");
    defaultsChanged = false;
  }
}
