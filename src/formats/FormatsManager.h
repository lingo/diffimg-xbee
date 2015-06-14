
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

#ifndef _FORMATSMANAGER_H_
#define _FORMATSMANAGER_H_

#include <QtCore/QObject>

namespace cv
{
    class Mat;
}

class BaseFormat;

class FormatsManager : public QObject
{
    Q_OBJECT

public:

    explicit FormatsManager(QObject *parent = 0);
    virtual ~FormatsManager();

    static void registerFormats(BaseFormat*);

    static const QList<BaseFormat *> & getFormats();

    static BaseFormat* getFormatByName(const QString &);

    static cv::Mat decode(const QString&file,bool &ok);

    static void clear();

private:

    static QList<BaseFormat *> m_Formats;
};

#endif // _FORMATSMANAGER_H_
