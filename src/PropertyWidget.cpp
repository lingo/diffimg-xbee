
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

#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtCore/QDate>

#include <climits>

#include "PropertyWidget.h"
#include "qtpropertymanager.h"
#include "BaseMetric.h"

PropertyWidget::PropertyWidget( QWidget *parent ) :
    QtTreePropertyBrowser( parent )
{
    m_variantManager = new QtVariantPropertyManager(this);
}

void PropertyWidget::displayData(BaseMetric *met)
{
    clear();
    if (!met)
        return;

    displayProperties(met);
    displayStatistics(met);
}

void PropertyWidget::displayProperties(BaseMetric *met)
{
    QtVariantProperty *parent;

    const QList<ImageProperty> &props = met->getProperties();

    // Range list
    parent = m_variantManager->addProperty( QVariant::String, tr("Properties") );
    parent->setValue("");
    addProperty(parent);

    foreach (const ImageProperty &prop, props)
    {
        QtVariantProperty *property = m_variantManager->addProperty( QVariant::String, prop.name );
        property->setValue(prop.value);
        property->setToolTip(prop.desc);
        property->setStatusTip(prop.value);
        parent->addSubProperty(property);
    }
}

void PropertyWidget::displayStatistics(BaseMetric *met)
{
    QtVariantProperty *property;
    QtVariantProperty *parent;

    const QList<MetricParam *> params = met->getOutputParams();

    // Range list
    parent = m_variantManager->addProperty( QVariant::String, tr("Statistics") );
    parent->setValue("");
    addProperty(parent);

    foreach (MetricParam * param, params)
    {
        if ( !param->isValid() )
            property = m_variantManager->addProperty( QVariant::String, param->name + " (*)" );
        else
            property = m_variantManager->addProperty( QVariant::String, param->name );
        if (param->value.type() == QMetaType::Double || param->value.type() == QMetaType::Float)
            property->setValue( QString().sprintf( "%.5f",param->value.toFloat() ) );
        else
            property->setValue(param->value);
        property->setToolTip(param->desc);
        parent->addSubProperty(property);
    }
}
