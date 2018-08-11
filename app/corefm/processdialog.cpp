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

#include "processdialog.h"


/**
 * @brief Creates dialog and connects it to process
 * @param proc process
 * @param procName name of program that is running in process
 * @param parent parent widget
 */
ProcessDialog::ProcessDialog(QProcess* proc, const QString &procName,
                             QWidget *parent) : QDialog(parent) {

  // Saves process
  this->procPtr = proc;

  // Size
  this->setMinimumSize(420, 320);
  this->setWindowTitle("CoreFM: " + procName);

  // Text
  QString text = tr("<b>Please wait custom action '%1' is in progress...</b>");

  // Create widgets
  output = new QListWidget(this);
  QLabel* labelText = new QLabel(text.arg(procName), this);
  QPushButton* btnClear = new QPushButton(tr("Clear"), this);
  QPushButton* btnClose = new QPushButton(tr("Hide"), this);
  QPushButton* btnTerminate = new QPushButton(tr("Terminate"), this);

  // Create buttons layout
  QHBoxLayout* layoutBtns = new QHBoxLayout();
  layoutBtns->setSpacing(10);
  layoutBtns->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding));
  layoutBtns->addWidget(btnClear);
  layoutBtns->addWidget(btnClose);
  layoutBtns->addWidget(btnTerminate);

  // Label properties
  labelText->setWordWrap(true);

  // Create main layout
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(labelText);
  layout->addItem(new QSpacerItem(0, 10));
  layout->addWidget(new QLabel(tr("Output:"), this));
  layout->addWidget(output);
  layout->addItem(layoutBtns);

  // Connect to process
  connect(procPtr, SIGNAL(started()), SLOT(onProcStarted()));
  connect(procPtr, SIGNAL(finished(int)), SLOT(onProcFinished()));
  connect(procPtr, SIGNAL(readyReadStandardOutput()), SLOT(onProcStdOut()));
  connect(btnTerminate, SIGNAL(clicked()), procPtr, SLOT(terminate()));
  connect(btnClear, SIGNAL(clicked()), output, SLOT(clear()));
  connect(btnClose, SIGNAL(clicked()), SLOT(hide()));
}

/**
 * @brief Reaction on process finish
 */
void ProcessDialog::onProcFinished()
{
  //this->hide();
  this->deleteLater();
}

/**
 * @brief Reaction on process start
 */
void ProcessDialog::onProcStarted()
{
  this->show();
}

/**
 * @brief Reaction on process standard output
 */
void ProcessDialog::onProcStdOut()
{
  QProcess* p = qobject_cast<QProcess*>(sender());
  QString text = QString::fromLocal8Bit(p->readAllStandardOutput());
  output->addItem(new QListWidgetItem(text, output));
  output->scrollToBottom();
}
