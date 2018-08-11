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

#include "applicationdialog.h"
#include "fileutils.h"


ApplicationDialog::ApplicationDialog(QWidget *parent) : QDialog(parent) {

  // Title and size
  this->setWindowTitle(tr("Select application"));
  this->setMinimumSize(320, 320);

  // Creates app list view
  appList = new QTreeWidget(this);
  appList->setIconSize(QSize(24, 24));
  appList->setAlternatingRowColors(true);
  appList->headerItem()->setText(0, tr("Application"));

  // Creates buttons
  QDialogButtonBox *buttons = new QDialogButtonBox(this);
  buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

  // Command bar
  edtCommand = new QLineEdit(this);
  edtCommand->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  QFormLayout* layoutCommand = new QFormLayout();
  layoutCommand->addRow(tr("Launcher: "), edtCommand);

  // Layout
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(appList);
  layout->addLayout(layoutCommand);
  layout->addWidget(buttons);

  // Synonyms for cathegory names
  catNames.clear();
  catNames.insert("Development", QStringList() << "Programming");
  catNames.insert("Games", QStringList() << "Game");
  catNames.insert("Graphics", QStringList());
  catNames.insert("Internet", QStringList() << "Network" << "WebBrowser");
  catNames.insert("Multimedia", QStringList() << "AudioVideo" << "Video");
  catNames.insert("Office", QStringList());
  catNames.insert("Other", QStringList());
  catNames.insert("Settings", QStringList() << "System");
  catNames.insert("Utilities", QStringList() << "Utility");

  // Load default icon
  defaultIcon = QIcon::fromTheme("application-x-executable");

  // Create default application cathegories
  categories.clear();
  createCategories();

  // Load applications and create category tree list
  QList<DesktopFile> apps = FileUtils::getApplications();
  foreach (DesktopFile app, apps) {

    // Check for name
    if (app.getName().compare("") == 0) {
      continue;
    }

    // Find category
    QTreeWidgetItem* category = findCategory(app);

    // Create item from current mime
    QTreeWidgetItem *item = new QTreeWidgetItem(category);
    item->setIcon(0, FileUtils::searchAppIcon(app, defaultIcon));
    item->setText(0, app.getName());
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    // Register application
    applications.insert(app.getPureFileName(), item);
  }

  // Create completer and its model for editation of command
  QStringListModel* model = new QStringListModel(this);
  model->setStringList(applications.keys());
  QCompleter* completer = new QCompleter(this);
  completer->setModel(model);
  edtCommand->setCompleter(completer);

  // Signals
  connect(appList,
          SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
          SLOT(updateCommand(QTreeWidgetItem*,QTreeWidgetItem*)));
}

/**
 * @brief Returns currently selected launcher
 * @return currently selected launcher
 */
QString ApplicationDialog::getCurrentLauncher() const {
  return edtCommand->text();
}

/**
 * @brief Creates default application categories
 * @param names names of cathegories with synonyms
 */
void ApplicationDialog::createCategories() {

  // Crate cathegories
  foreach (QString name, catNames.keys()) {

    // Find icon
    QIcon icon = QIcon::fromTheme("applications-" + name.toLower());

    // If icon not found, check synonyms
    if (icon.isNull()) {
      foreach (QString synonym, catNames.value(name)) {
        icon = QIcon::fromTheme("applications-" + synonym.toLower());
        break;
      }
    }

    // If icon still not found, retrieve default icon
    if (icon.isNull()) {
      icon = defaultIcon;
    }

    // Create category
    QTreeWidgetItem* category = new QTreeWidgetItem(appList);
    category->setText(0, name);
    category->setIcon(0, icon);
    category->setFlags(Qt::ItemIsEnabled);
    categories.insert(name, category);
  }
}

/**
 * @brief Searches the most suitable category for application
 * @param app
 * @return cathegory
 */
QTreeWidgetItem* ApplicationDialog::findCategory(const DesktopFile &app) {

  // Default categoty is 'Other'
  QTreeWidgetItem* category = categories.value("Other");

  // Try to find more suitable category
  foreach (QString name, catNames.keys()) {

    // Try cathegory name
    if (app.getCategories().contains(name)) {
      category = categories.value(name);
      break;
    }

    // Try synonyms
    bool found = false;
    foreach (QString synonym, catNames.value(name)) {
      if (app.getCategories().contains(synonym)) {
        found = true;
        break;
      }
    }
    if (found) {
      category = categories.value(name);
      break;
    }
  }
  return category;
}

/**
 * @brief Updates launcher command
 * @param current
 * @param previous
 */
void ApplicationDialog::updateCommand(QTreeWidgetItem *current,QTreeWidgetItem *previous) {
    Q_UNUSED(previous);
    edtCommand->setText(applications.key(current));
}
