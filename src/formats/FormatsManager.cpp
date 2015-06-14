
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

#include <QtCore/QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FormatsManager.h"
#include "BaseFormat.h"

QList<BaseFormat *> FormatsManager::m_Formats;

FormatsManager::FormatsManager(QObject *parent) : QObject(parent)
{
}

FormatsManager::~FormatsManager()
{
    clear();
}

void FormatsManager::clear()
{
    qDeleteAll(m_Formats);
    m_Formats.clear();
}

void FormatsManager::registerFormats(BaseFormat *Format)
{
    m_Formats << Format;
}

const QList<BaseFormat *> & FormatsManager::getFormats()
{
    return m_Formats;
}

BaseFormat* FormatsManager::getFormatByName(const QString &name)
{
    foreach (BaseFormat *fmt , m_Formats)
    {
        if (fmt->getName() == name)
            return fmt;
    }
    return NULL;
}

cv::Mat FormatsManager::decode(const QString &file,bool &ok)
{
    foreach (BaseFormat *fmt , m_Formats)
    {
        if (fmt->accept(file))
        {
            return fmt->decode(file,ok);
        }
    }
    ok = false;
    return cv::Mat();
}