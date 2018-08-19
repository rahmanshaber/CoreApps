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

#include "corepdf.h"


corepdf::corepdf(QWidget *parent):QWidget(parent)
{

    // set window size
    int x = static_cast<int>(Utilities::screensize().width()  * .8);
    int y = static_cast<int>(Utilities::screensize().height()  * .7);
    this->resize(x, y);

    QVBoxLayout * mainLayout = new QVBoxLayout();
    PdfWidget = new QPdfWidget();
    setWindowTitle("CorPDF");
    setWindowIcon(QIcon(":/app/icons/app-icons/CorePDF.svg"));
    setObjectName("corepdf");
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(PdfWidget);
    setLayout(mainLayout);
}

corepdf::~corepdf()
{
    delete PdfWidget;
}


void corepdf::openPdfFile(const QString path)
{
    workFilePath = path;

    connect(PdfWidget, &QPdfWidget::initialized, [this,path]() {
//        PdfWidget->setToolbarVisible(false);
        QFile f(path);
//        qDebug() << "CorePDF" << path;
        if (f.open(QIODevice::ReadOnly)) {
            QByteArray data = f.readAll();
            PdfWidget->loadData(data);
            f.close();
        }
    });

}

void corepdf::closeEvent(QCloseEvent *event)
{
    event->ignore();
    // Function from utilities.cpp
    Utilities::saveToRecent("CorePDF", workFilePath);
    QPdfWidget *cpdf = qobject_cast<QPdfWidget*>(this->children().at(1));
    cpdf->closeDocument();
    event->accept();
}

void corepdf::sendFiles(const QStringList &paths) {
    if (paths.count()) {
        openPdfFile(Utilities::checkIsValidFile(paths.at(0)));
    }
}
