
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

#ifndef _DIALOGFILES_H_
#define _DIALOGFILES_H_

#include <QtGui/QWidget>
#include "ui_FilesDialog.h"

using namespace Ui;

class FilesDialog : public QDialog, public FilesDialogClass
{
    Q_OBJECT

public:

    FilesDialog(QWidget *parent = 0);
    ~FilesDialog();

    void getCurrentFiles(QString &file1,QString &file2);

    void setFile1(const QString &);
    void setFile2(const QString &);

public slots:

    void on_pushButtonFile1_pressed();
    void on_pushButtonFile2_pressed();
    void on_lineEditFile1_textChanged ( const QString & text );
    void on_lineEditFile2_textChanged ( const QString & text );

private:

    void getPath(const QString &f1,const QString &f2);
    void validPath(const QString &path, QWidget *w);

    QString m_path;
};

#endif // _DIALOGFILES_H_
