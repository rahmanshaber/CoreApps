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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QVariant>
#include <QObject>
#include <QMap>
#include <QTextStream>
#include <QStringList>
#include <QFile>

/**
 * @class Properties
 * @brief Read property files
 * @author Michal Rost
 * @date 26.1.2013
 */
class Properties {

public:
  explicit Properties(const QString &fileName = "", const QString &group = "");
  explicit Properties(const Properties &other);
  QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
  bool load(const QString &fileName, const QString &group = "");
  bool save(const QString &fileName, const QString &group = "");
  void set(const QString &key, const QVariant &value);
  bool contains(const QString &key) const;
  QStringList getKeys() const;

protected:
  QMap<QString, QVariant> data;

};

#endif // PROPERTIES_H
