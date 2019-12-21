
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

#include "AboutDialog.h"
#include "MiscFunctions.h"
#include "AppSettings.h"
#include "LogHandler.h"

#include "MetricsManager.h"
#include "BaseMetric.h"

#include "WipeMethod.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QTranslator>

#include <QMessageBox>
#include <QIntValidator>
#include <QDoubleValidator>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent),
    m_dValidator( new QDoubleValidator(this) ),
    m_iValidator( new QIntValidator(this) )
{
    setupUi(this);
    updateAbout();
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::updateAbout()
{
    // setup credits ...
    updateCredits();

    // setup changelog ...
    updateChangelog();

    // setup image format list
    updateInfosImageFormats();

    // setup user image format parameters
    updateUserImageParameterLanguages();

    // setup languages list
    updateInfosLanguages();

    // setup pref language
    updatePrefsLanguages();

    // setup wipe effect options
    updateWipeEffects();

    // init combobox metrics
    initMetrics();

    labelRelease->setText( QString(PACKAGE_NAME) + " " + QString(PACKAGE_VERSION) );
}

void AboutDialog::updateWipeEffects()
{
    comboBoxWipeEffectAxis->clear();
    comboBoxWipeEffectAxis->addItem(tr("Horizontal effect"),WipeMethod::WIPE_HORIZONTAL);
    comboBoxWipeEffectAxis->addItem(tr("Vertical effect"),WipeMethod::WIPE_VERTICAL);
}

void AboutDialog::updateUserImageParameterLanguages()
{
    comboBoxImageFormatName->clear();

    // update comboBoxUploaders
    //const QList<BaseFormat *> &list = FormatsManager::getFormats();
    //foreach (BaseFormat * fmt, list)
    //{
    //    comboBoxImageFormatName->addItem( fmt->getName(),qVariantFromValue( (void *) fmt ) ); // store directly pointer
    //}

}

void AboutDialog::updateInfosImageFormats()
{
    // update format list

    // clear the current list
    listWidgetFormats->clear();

    QStringList formats = MiscFunctions::getAvailablesImageFormatsList();
    QMap<QString, QString> longFormats = MiscFunctions::getLongImageFormats();

    foreach (const QString &format, formats)
    {
        if ( longFormats.contains(format) )
            new QListWidgetItem(longFormats[format] + " (*." + format + ")", listWidgetFormats);
        else
            new QListWidgetItem(format, listWidgetFormats);
    }

    // Qt version
    labelQtVersion->setText( tr("Qt version %1").arg(QT_VERSION_STR) );
}

void AboutDialog::updateInfosLanguages()
{
    // get the current Language from settings
    AppSettings settings;
    settings.beginGroup("Application");
    m_currentLanguage = settings.value("currentLanguage","auto").toString();
    QString currentLang = m_currentLanguage;

    if (currentLang.isEmpty() || currentLang == "auto")
        currentLang = QLocale::system().name().left(2);

    // update translation list
    const QMap<QString, QString> &mapLang = MiscFunctions::getAvailableLanguages();
    QStringList names = mapLang.keys();
    listWidgetTranslations->clear();
    foreach (const QString &lang, names)
    {
        QString text;

        if (mapLang[lang] == currentLang)
            text = lang + " *";
        else
            text = lang;

        QListWidgetItem *item = new QListWidgetItem(text, listWidgetTranslations);
        item->setData(Qt::UserRole,mapLang[lang]); // save the lang acronym
    }
}

void AboutDialog::updateCredits()
{
    fillTextedit("CREDITS.txt",textEditCredits);
}

void AboutDialog::updateChangelog()
{
    fillTextedit("Changelog.txt",textEditChangelog);
}

void AboutDialog::fillTextedit(const QString &file, QTextEdit *text)
{
    text->viewport ()->setAutoFillBackground(false);
    QStringList paths;
    QString filePath;
    paths << QApplication::applicationDirPath () + "/" + file;
    paths << QApplication::applicationDirPath () + "/../" + file;
    paths << QApplication::applicationDirPath () + "/../../" + file;
    paths << QApplication::applicationDirPath () + "/../../../" + file;
    paths << QApplication::applicationDirPath () + "/../Resources/" + file; // MaxOSX
    paths << "/usr/share/" + QString("%1").arg(PACKAGE_NAME).toLower() + "/" + file;
    paths << "/usr/local/share/" + QString("%1").arg(PACKAGE_NAME).toLower() + "/" + file;

    foreach (const QString &path, paths)
    {
        if ( QFileInfo(path).exists() )
        {
            filePath = path;
            break;
        }
    }

    // perhaps, not found !!
    if ( QFileInfo(filePath).exists() )
    {
        QFile file( filePath );
        if( file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            text->setText( file.readAll() );
            file.close();
            return;
        }
    }
    text->setText( tr("The %1 file can't be found, sorry ...").arg(file) );
}

void AboutDialog::on_pushButtonClearLog_pressed()
{
    textBrowserLog->clear();
}

void AboutDialog::showAbout()
{
    tabWidget->setCurrentIndex (0);
}

void AboutDialog::showPreferences()
{
    tabWidget->setCurrentIndex (1);
}

void AboutDialog::updatePrefsLanguages()
{
    comboBoxLanguage->clear();

    // set detected language
    QString detectedLanguage = QLocale::system().name().left(2);
    labelDetectedLanguage->setText( tr("detected language: %1").arg(detectedLanguage) );

    // fill comboBox
    const QMap<QString, QString> &mapLang = MiscFunctions::getAvailableLanguages();
    QStringList names = mapLang.keys();
    comboBoxLanguage->addItem(tr("Automatic detection"), "auto");
    if (m_currentLanguage == "auto")
        comboBoxLanguage->setCurrentIndex(comboBoxLanguage->count() - 1);
    comboBoxLanguage->addItem(tr("Default (no use of translation files)"),"default");
    if (m_currentLanguage == "default")
        comboBoxLanguage->setCurrentIndex(comboBoxLanguage->count() - 1);
    foreach (const QString &lang, names)
    {
        comboBoxLanguage->addItem(lang,mapLang[lang]);
        if (mapLang[lang] == m_currentLanguage)
            comboBoxLanguage->setCurrentIndex(comboBoxLanguage->count() - 1);
    }
}

void AboutDialog::setCurrentLanguage(const QString &lang)
{
    comboBoxLanguage->setCurrentIndex( comboBoxLanguage->findData(lang) );
}

void AboutDialog::on_comboBoxLanguage_activated(int index)
{
    QString lang = comboBoxLanguage->itemData(index).toString();
    if (lang == m_currentLanguage)
        return;

    m_currentLanguage = lang;

    // check if recursive scan
    int ret = QMessageBox::question(this, PACKAGE_NAME,
                                    tr("The application need to restart in order to take into account new translation\n, Do you want to restart application now ?"),
                                    QMessageBox::Yes
                                    | QMessageBox::No);

    if (ret == QMessageBox::Yes)
    {
        emit accepted ();
        emit restart();
    }
}

void AboutDialog::initMetrics()
{
    comboBoxMetrics->clear();
    // update comboBoxUploaders
    const QList<BaseMetric *> &list = MetricsManager::getMetrics();
    foreach (BaseMetric * met, list)
    {
        comboBoxMetrics->addItem( met->getName(),qVariantFromValue( (void *) met ) ); // store directly pointer
    }
}

void AboutDialog::on_listWidgetTranslations_itemClicked( QListWidgetItem * item)
{
    QString lang = item->data(Qt::UserRole).toString();
    QString file( MiscFunctions::getTranslationsPath("fr") );
    file += QString("/%1_%2.qm").arg( QString(PACKAGE_NAME).toLower() ).arg(lang);

    QTranslator translator;
    translator.load(file);
    labelTranslation->setText( translator.translate("","release of translation and translator name please","put your name here dear translator and the release of the translation file!!") );
}

void AboutDialog::on_comboBoxImageFormatName_currentIndexChanged(int index)
{
    labelFormatDesc->setText("");

    if (index < 0)
        return;

    //BaseFormat *fmt = static_cast<BaseFormat *>( comboBoxImageFormatName->itemData(index,Qt::UserRole).value<void *>() );
    //labelFormatDesc->setText( fmt->getDesc() );

    //// update comboBoxFormatParam
    //comboBoxFormatParam->clear();
    //const QList<FormatProperty*> &listInput = fmt->getProperties();

    //lineEditFormatParam->setText("");
    //lineEditFormatParam->setToolTip("");
    //lineEditFormatParam->setEnabled( !listInput.isEmpty() );
    //comboBoxFormatParam->setToolTip("");
    //comboBoxcomboBoxFormatParamValues->setToolTip("");

    //foreach (FormatProperty * prop, listInput)
    //{
    //    comboBoxFormatParam->addItem( prop->name,qVariantFromValue( (void *) prop ) ); // store directly pointer
    //}

}

void AboutDialog::on_comboBoxFormatParam_currentIndexChanged(int index)
{
    setupFormatParam(index,comboBoxFormatParam,lineEditFormatParam,comboBoxcomboBoxFormatParamValues,pushButtonApplyUserParam);
}

void AboutDialog::on_comboBoxMetrics_currentIndexChanged(int index)
{
    labelMetricPixmap->setPixmap( QPixmap() );
    labelMetricDesc->setText("");

    if (index < 0)
        return;

    BaseMetric *met = static_cast<BaseMetric *>( comboBoxMetrics->itemData(index,Qt::UserRole).value<void *>() );
    labelMetricPixmap->setPixmap( met->getLogo().scaled(QSize(48,48),Qt::KeepAspectRatio,Qt::SmoothTransformation) );
    labelMetricDesc->setText( met->getDesc() );

    // update comboBoxInputParam
    comboBoxInputParam->clear();
    const QList<MetricParam *> &listInput = met->getInputParams();

    lineEditInputParam->setText("");
    lineEditInputParam->setToolTip("");
    lineEditInputParam->setEnabled( !listInput.isEmpty() );
    comboBoxInputParam->setToolTip("");
    pushButtonApplyInputParam->setEnabled(false);

    foreach (MetricParam * param, listInput)
    {
        comboBoxInputParam->addItem( param->name,qVariantFromValue( (void *) param ) ); // store directly pointer
    }

    // update comboBoxOutputParameter
    comboBoxOutputParam->clear();
    const QList<MetricParam *> &listOutput = met->getOutputParams();

    lineEditThreshold->setText("");
    lineEditThreshold->setToolTip("");
    lineEditThreshold->setEnabled( !listOutput.isEmpty() && groupBoxEnableThreshold->isChecked() );
    comboBoxOutputParam->setToolTip("");

    foreach (MetricParam * param, listOutput)
    {
        comboBoxOutputParam->addItem( param->name,qVariantFromValue( (void *) param ) ); // store directly pointer
        if (param->used)
            comboBoxOutputParam->setCurrentIndex(comboBoxOutputParam->count() - 1);
    }
}

void AboutDialog::on_groupBoxEnableThreshold_toggled ( bool on )
{
    BaseMetric *met = static_cast<BaseMetric *>( comboBoxMetrics->itemData(comboBoxMetrics->currentIndex(),Qt::UserRole).value<void *>() );
    if (met)
    {
        const QList<MetricParam *> &list = met->getOutputParams();
        lineEditThreshold->setEnabled(!list.isEmpty() && on);
    }
}

void AboutDialog::on_comboBoxInputParam_currentIndexChanged(int index)
{
    setupParam(index,comboBoxInputParam,lineEditInputParam,pushButtonApplyInputParam);
}

void AboutDialog::on_comboBoxOutputParam_currentIndexChanged(int index)
{
    setupParam(index,comboBoxOutputParam,lineEditThreshold,pushButtonApplyImageParam);

    MetricParam *param = static_cast<MetricParam *>( comboBoxOutputParam->itemData(index,Qt::UserRole).value<void *>() );
    BaseMetric *met = static_cast<BaseMetric *>( comboBoxMetrics->itemData(comboBoxMetrics->currentIndex(),Qt::UserRole).value<void *>() );
    met->setDiscriminatingParam(param);
}
void AboutDialog::setupFormatParam(int index, QComboBox *combo, QLineEdit *lineEdit,QComboBox *comboEdit, QPushButton * )
{
#if 0
    if (index < 0)
        return;

    FormatProperty *prop = static_cast<FormatProperty *>( combo->itemData(index,Qt::UserRole).value<void *>() );

    lineEdit->setToolTip(prop->desc);
    comboEdit->setToolTip(prop->desc);
    combo->setToolTip(prop->desc);

    comboEdit->setVisible(!prop->availableValues.isEmpty());
    lineEdit->setVisible(prop->availableValues.isEmpty());

    if (prop->availableValues.isEmpty())
    {   
        lineEdit->setText( prop->value.toString() );
    }
    else
    {
        comboEdit->clear();
        foreach (const QString&key , prop->availableValues)
            comboEdit->addItem(key);

        comboEdit->setCurrentIndex(comboEdit->findText( prop->value.toString()));
    }
#endif
}

void AboutDialog::setupParam(int index, QComboBox *combo, QLineEdit *lineEdit, QPushButton *button )
{
    if (index < 0)
        return;

    MetricParam *param = static_cast<MetricParam *>( combo->itemData(index,Qt::UserRole).value<void *>() );
    combo->setToolTip(param->desc);
    lineEdit->setText( param->threshold.toString() );
    lineEdit->setToolTip(param->desc);

    // update the validator
    switch ( param->threshold.type() )
    {
        case QMetaType::Int:
        {
            lineEdit->setValidator(m_iValidator);
            break;
        }
        case QMetaType::Double:
        case  QMetaType::Float:
        {
            lineEdit->setValidator(m_dValidator);
            break;
        }
        default:
            lineEdit->setValidator(NULL);
            break;
    }

    // restore the standard palette
    changePalette(false,lineEdit,button );
}

void AboutDialog::on_lineEditInputParam_textEdited ( const QString & text )
{
    Q_UNUSED(text);
    changePalette(true,lineEditInputParam,pushButtonApplyInputParam);
}

void AboutDialog::on_lineEditThreshold_textEdited ( const QString & text )
{
    Q_UNUSED(text);
    changePalette(true,lineEditThreshold,pushButtonApplyImageParam);
}

void AboutDialog::on_pushButtonApplyInputParam_pressed()
{
    applyParam(comboBoxInputParam, lineEditInputParam, pushButtonApplyInputParam );
}

void AboutDialog::on_pushButtonApplyUserParam_pressed()
{
    applyFormatProperty(comboBoxFormatParam, lineEditFormatParam, comboBoxcomboBoxFormatParamValues, pushButtonApplyUserParam );
}

void AboutDialog::applyFormatProperty(QComboBox *combo, QLineEdit *lineEdit, QComboBox *comboValue, QPushButton * )
{
    //changePalette(true,lineEdit,button);

    // apply the modification
#if 0
    FormatProperty *prop = static_cast<FormatProperty *>( combo->itemData(combo->currentIndex(),Qt::UserRole).value<void *>() );

    if (!prop)
        return;

    if (prop->availableValues.isEmpty())
        prop->value = lineEdit->text().toLower();
    else
        prop->value = comboValue->currentText();
#endif
}

void AboutDialog::on_pushButtonApplyImageParam_pressed()
{
    applyParam(comboBoxOutputParam, lineEditThreshold, pushButtonApplyImageParam );
}

void AboutDialog::applyParam(QComboBox *combo, QLineEdit *lineEdit, QPushButton *button )
{
    changePalette(false,lineEdit,button);

    // apply the modification
    MetricParam *param = static_cast<MetricParam *>( combo->itemData(combo->currentIndex(),Qt::UserRole).value<void *>() );
    QString valText = lineEdit->text();
    if (param)
    {
        // update the theshold

        /*
           switch ( param->threshold.type() )
           {
            case QVariant::Int:
            {
                param->threshold = valText.toInt();
                break;
            }
            case QVariant::Double:
            case  QMetaType::Float:
            {
                param->threshold = valText.toFloat();
                break;
            }
            default:
                break;
           }
         */
        param->setThreshold(valText);
    }
}

void AboutDialog::changePalette(bool modified, QLineEdit *lineEdit, QPushButton *button)
{
    button->setEnabled(modified);
    if (modified)
    {
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::red);
        lineEdit->setPalette(pal);
    }
    else
    {
        // restore normal palette
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::black);
        lineEdit->setPalette(pal);
    }
}

void AboutDialog::on_tabWidget_currentChanged ( int index )
{
    setWindowTitle( tabWidget->tabText(index) );
}
