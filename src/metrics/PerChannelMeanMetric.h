
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
#ifndef _BYCHANNELMEANMETRIC_H_
#define _BYCHANNELMEANMETRIC_H_

#include "BaseMetric.h"

class PerChannelMeanMetric : public BaseMetric
{
    Q_OBJECT

public:

    explicit PerChannelMeanMetric(QObject *parent = 0);
    virtual ~PerChannelMeanMetric();

    virtual QPixmap getLogo() const;

signals:

protected:

    virtual void performDifference();
};

#endif // _BYCHANNELMEANMETRIC_H_
