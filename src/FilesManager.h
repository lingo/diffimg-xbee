
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

#ifndef _FILESMANAGER_H_
#define _FILESMANAGER_H_

#include <QtCore/QString>
#include <QtCore/QStringList>

//! Class for manage proposed filename across widget

class FilesManager
{
public:

    static void getCurrentFiles(QString &file1,QString &file2);
    static void getNextFiles(QString &file1,QString &file2);
    static void getPrevFiles(QString &file1,QString &file2);
    static const QStringList & getFilelist1();
    static const QStringList & getFilelist2();

private:

    static QString getValidFile(const QString &basename,const QString &refpath, const QStringList &refList);
    static void getFollowingFiles(QString &file1,QString &file2, int increment = 1);

    static void rescan1(const QString &);
    static void rescan2(const QString &);
    static void rescan(const QString &file, QString & refPath, QStringList &refList);

    static QStringList m_lFiles1;
    static QString m_refPath1;
    static QString m_refPath2;
    static QStringList m_lFiles2;
};

#endif // _FilesManager_H_
