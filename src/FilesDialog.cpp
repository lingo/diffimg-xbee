
/******************************************************************************
   DiffImg: image difference viewer
   Copyright(C) 2011-2014  xbee@xbee.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *******************************************************************************/

#include <QtCore/QFileInfo>

#include <QFileDialog>
#include <QCompleter>
#include <QDirModel>

#include "FilesDialog.h"
#include "MiscFunctions.h"
#include "LogHandler.h"
#include "FilesManager.h"

FilesDialog::FilesDialog(QWidget *parent)
    : QDialog(parent)
{
    Q_INIT_RESOURCE(diffimg);
    setupUi(this);

    // add completer to line edit
    QCompleter *completerDir = new QCompleter(this);
    QDirModel *dirModelDir = new QDirModel(completerDir);
    dirModelDir->setFilter(QDir::Dirs);
    completerDir->setModel(dirModelDir);
    lineEditFile1->setCompleter(completerDir);
    lineEditFile2->setCompleter(completerDir);
}

FilesDialog::~FilesDialog()
{
}

void FilesDialog::getPath(const QString &f1, const QString &f2)
{
    // get the directory
    if (!f1.isEmpty()) {
        QFileInfo fi(f1);
        m_path = fi.absolutePath();
    } else if (!f2.isEmpty()) {
        QFileInfo fi(f2);
        m_path = fi.absolutePath();
    }
}

void FilesDialog::on_pushButtonFile1_pressed()
{
    getPath(lineEditFile1->text(), lineEditFile2->text());

    QString formats = MiscFunctions::getAvailablesImageFormats();
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"),
                   m_path,
                   formats);

    if (!file.isEmpty()) {
        lineEditFile1->setText(file);
    }
}

void FilesDialog::on_pushButtonFile2_pressed()
{
    getPath(lineEditFile2->text(), lineEditFile1->text());

    QString formats = MiscFunctions::getAvailablesImageFormats();
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"),
                   m_path,
                   formats);

    if (!file.isEmpty()) {
        lineEditFile2->setText(file);
    }
}

void FilesDialog::on_lineEditFile1_textChanged(const QString &text)
{
    validPath(text, lineEditFile1);
}

void FilesDialog::on_lineEditFile2_textChanged(const QString &text)
{
    validPath(text, lineEditFile2);
}

void FilesDialog::validPath(const QString &path, QWidget *w)
{
    if (!w) {
        return;
    }

    if (!QFileInfo(path).exists()) {
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::red);
        w->setPalette(pal);
    } else {
        // restore normal palette
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::black);
        w->setPalette(pal);
    }
}

void FilesDialog::getCurrentFiles(QString &file1, QString &file2)
{
    file1 = lineEditFile1->text();
    file2 = lineEditFile2->text();

    FilesManager::getCurrentFiles(file1, file2);
}

void FilesDialog::setFile1(const QString &f)
{
    lineEditFile1->setText(f);
}

void FilesDialog::setFile2(const QString &f)
{
    lineEditFile2->setText(f);
}
