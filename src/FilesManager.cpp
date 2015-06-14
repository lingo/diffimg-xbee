
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
#include <QtCore/QDir>

#include "FilesManager.h"
#include "MiscFunctions.h"
#include "LogHandler.h"

QStringList FilesManager::m_lFiles1;
QString FilesManager::m_refPath1;
QString FilesManager::m_refPath2;
QStringList FilesManager::m_lFiles2;

void FilesManager::getPrevFiles(QString &file1,QString &file2)
{
    getFollowingFiles(file1,file2,-1);
}

void FilesManager::getNextFiles(QString &file1,QString &file2)
{
    getFollowingFiles(file1,file2);
}

void FilesManager::getFollowingFiles(QString &file1,QString &file2, int increment)
{
    QFileInfo fi1(file1);
    QFileInfo fi2(file2);

    // reference is always 1
    int currIndex = m_lFiles1.indexOf( fi1.fileName () );

    if ( currIndex>=0 && !m_lFiles1.isEmpty() )
    {
        int loop = 0;
        bool ok = false;
        currIndex += increment;
        while (!ok)
        {
            if (increment>0)
            {
                currIndex %= m_lFiles1.size();
            }
            else
            {
                if (currIndex < 0)
                    currIndex = m_lFiles1.size() - 1;
            }

            file1 = m_refPath1 + "/" + m_lFiles1[(currIndex) % m_lFiles1.size()];
            file2 = getValidFile(QFileInfo(file1).fileName(),m_refPath2,m_lFiles2);

            if ( !file2.isEmpty() )
                ok = true;

            loop++;
            currIndex += increment;

            if ( loop > 2 * m_lFiles1.size() )
            {
                ok = true;
                file1 = "";
                file2 = "";
                break;
            }
        }
    }
}

void FilesManager::rescan(const QString &file, QString &refPath, QStringList &refList)
{
    bool rescan = false;
    if ( QFileInfo(file).isDir() )
    {
        if (refPath != file)
        {
            refPath = file;
            rescan = true;
        }
    }
    else if ( QFileInfo(file).isFile() )
    {
        if (QFileInfo(file).absolutePath() != refPath )
        {
            refPath = QFileInfo(file).absolutePath();
            rescan = true;
        }
    }

    if (rescan)
    {
        QString formats = MiscFunctions::getAvailablesImageFormats();
        refList = QDir(refPath).entryList(formats.split(" "), QDir::Files | QDir::Readable,QDir::Name);
    }
}

void FilesManager::rescan1(const QString &file)
{
    rescan(file,m_refPath1,m_lFiles1);
}

void FilesManager::rescan2(const QString &file)
{
    rescan(file,m_refPath2,m_lFiles2);
}

QString FilesManager::getValidFile(const QString &basename, const QString &refPath, const QStringList &refList)
{
    // file exist in the second directory ?
    if ( refList.contains(basename) )
        return refPath + "/" + basename;

    // try to find a file with another suffix
    QRegExp rx( QString("^%1.*").arg( QFileInfo(basename).completeBaseName() ) );
    int index = refList.indexOf(rx);
    if (index >= 0)
        return refPath + "/" + refList.at(index);

    return "";
}

void FilesManager::getCurrentFiles(QString &file1,QString &file2)
{
    // basic check
    if ( QFileInfo(file1).isDir() && QFileInfo(file2).isDir() )
    {
        LogHandler::getInstance()->reportError( QString("Can't compare two directory, only two files or a file and a directory (%1/%2)").arg( file1 ).arg(file2) );
        file1 = "";
        file2 = "";
        return;
    }

    rescan1(file1);
    rescan2(file2);

    if ( QFileInfo(file1).isDir() )
    {
        // try to find the same basename
        file1 = getValidFile(QFileInfo(file2).fileName(),m_refPath1, m_lFiles1);
    }

    if ( QFileInfo(file2).isDir() )
    {
        // try to find the same basename
        file2 = getValidFile(QFileInfo(file1).fileName(),m_refPath2,m_lFiles2);
    }
}

const QStringList & FilesManager::getFilelist1()
{
    return m_lFiles1;
}

const QStringList & FilesManager::getFilelist2()
{
    return m_lFiles2;
}
