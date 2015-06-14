
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
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <opencv2/imgproc/imgproc.hpp>

#include "BaseFormat.h"
#include "LogHandler.h"
#include "AppSettings.h"
#include "MiscFunctions.h"

//---------------------------------------------------------------------------

BaseFormat::BaseFormat(QObject *parent) :
    QObject(parent),
    m_type("Unknown"),
    m_name( tr("Unknown") ),
    m_desc( tr("No description") ),
    m_init(false)
{
}

BaseFormat::~BaseFormat()
{
    saveSettings();
    qDeleteAll(m_properties);
}

void BaseFormat::init()
{
    if (m_init)
        return;

    loadSettings();
    m_init = true;
}

const QString & BaseFormat::getType() const
{
    return m_type;
}

const QString & BaseFormat::getName() const
{
    return m_name;
}

const QString & BaseFormat::getDesc() const
{
    return m_desc;
}

const QList<FormatProperty *> & BaseFormat::getProperties() const
{
    return m_properties;
}

void BaseFormat::loadSettings()
{
    AppSettings settings;

    settings.beginGroup("Formats");

    settings.beginGroup(m_type);

    settings.beginGroup("Param");

    foreach (FormatProperty * prop, m_properties)
    {
        prop->value = settings.value(prop->name,"").toString();
    }
 

    settings.endGroup();

    settings.endGroup();
    settings.endGroup();
}

void BaseFormat::saveSettings()
{
    AppSettings settings;

    settings.beginGroup("Formats");

    settings.beginGroup(m_type);

    settings.beginGroup("Param");


    foreach (FormatProperty * prop, m_properties)
        settings.setValue(prop->name,prop->value);

    settings.endGroup();


    settings.endGroup();
    settings.endGroup();
}
