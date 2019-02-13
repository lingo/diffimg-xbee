
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

#ifndef _BASEFORMAT_H_
#define _BASEFORMAT_H_

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QPixmap>

namespace cv
{
    class Mat;
}

struct FormatProperty
{
    FormatProperty(const QString & n, const QString &d, const QVariant &v) :
name(n),
desc(d),
value(v)
{
}

QString name;
QString desc;
QVariant value;
QStringList availableValues;
};

class FormatParam
{
public:

    FormatParam(const QString &t, const QString &n, const QString &d, const QVariant &v) :
      type(t),
          name(n),
          desc(d),
          value(v),
          defaultValue(v)
      {
      }

      QString type;
      QString name;
      QString desc;
      QVariant value;
      QVariant defaultValue;
      bool used;
};

class BaseFormat : public QObject
{
    Q_OBJECT

public:

    explicit BaseFormat(QObject *parent = 0);
    virtual ~BaseFormat();

    const QString & getType() const;
    const QString & getName() const;
    const QString & getDesc() const;

    const QList<FormatProperty *> & getProperties() const;

    virtual cv::Mat decode(const QString &file, bool &ok) = 0;
    virtual bool accept(const QString&file) = 0;

public slots:

protected slots:

protected:

    void saveSettings();
    void loadSettings();
    void init();

    QString m_type;
    QString m_name;
    QString m_desc;

    // properties = size, type, ...
    QList<FormatProperty *> m_properties;
    bool m_init;

};

#endif // _BASEFORMAT_H_
