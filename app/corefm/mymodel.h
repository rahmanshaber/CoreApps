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

#ifndef MYMODEL_H
#define MYMODEL_H

#include "mymodelitem.h"

#include <QtGui>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <QApplication>


QString formatSize(qint64);

/**
 * @class myModel
 * @brief holding information about files in FS
 * @author Wittfella
 */
class myModel : public QAbstractItemModel {
  Q_OBJECT
public:

  /**
   * @enum DragMode
   * @brief Represents drag mode
   */
  enum DragMode {
    DM_UNKNOWN = 0,
    DM_COPY,
    DM_MOVE,
    DM_LINK
  };

  myModel(bool realMime);
  ~myModel();
  void loadMimeTypes() const;
  void cacheInfo();
  void setMode(bool);
  void loadThumbs(QModelIndexList);
  void addCutItems(QStringList);
  void populateItem(myModelItem *item);
  void fetchMore(const QModelIndex & parent);
  void refresh();
  void refreshItems();
  void update();
  bool remove(const QModelIndex & index );
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                    int column, const QModelIndex &parent);
  bool isDir(const QModelIndex &index);
  bool canFetchMore (const QModelIndex & parent) const;
  bool setRootPath(const QString& path);
  void setRealMimeTypes(bool realMimeTypes);
  bool isRealMimeTypes() const;
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex index(const QString& path) const;
  QModelIndex parent(const QModelIndex &index) const;
  QModelIndex insertFolder(QModelIndex parent);
  QModelIndex insertFile(QModelIndex parent);
  int rowCount(const QModelIndex &parent) const;
  qint64 size(const QModelIndex &index);
  QString fileName(const QModelIndex &index);
  QString filePath(const QModelIndex &index);
  QString getMimeType(const QModelIndex &index);
  QStringList mimeTypes() const;
  QByteArray getThumb(QString item);
  QFileInfo fileInfo(const QModelIndex &index);
  Qt::DropActions supportedDropActions () const;
  QMimeData* mimeData(const QModelIndexList & indexes) const;
  QHash<QString,QIcon> *mimeIcons;
  QHash<QString,QIcon> *folderIcons;
  QCache<QString,QIcon> *icons;

public slots:
  void notifyChange();
  void notifyProcess(int eventID);
  void eventTimeout();
  void addWatcher(myModelItem* path);
  void clearCutItems();
  void clearIconCache();

signals:
  void dragDropPaste(const QMimeData *data, QString newPath,myModel::DragMode mode = DM_UNKNOWN);
  void thumbUpdate(const QModelIndex index);

protected:
  QVariant data(const QModelIndex & index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  bool setData(const QModelIndex & index, const QVariant & value,
               int role = Qt::EditRole);
  int columnCount(const QModelIndex &parent) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant findIcon(myModelItem *item) const;
  QVariant findMimeIcon(myModelItem *item) const;

private:
  bool realMimeTypes;
  bool showThumbs;
  int thumbCount;

  QPalette colors;
  QStringList cutItems;
  QHash<QString,QString> *mimeGlob;
  QHash<QString,QString> *mimeGeneric;
  QHash<QString,QByteArray> *thumbs;
  QMimeDatabase mimetype;

  myModelItem* rootItem;
  QString currentRootPath;
  QFileIconProvider* iconFactory;

  int inotifyFD;
  QSocketNotifier *notifier;
  QHash<int, QString> watchers;
  QTimer eventTimer;
  int lastEventID;
};

#endif // MYMODEL_H
