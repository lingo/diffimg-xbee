
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

#ifndef _DIALOGABOUT_H_
#define _DIALOGABOUT_H_

#include <QDialog>
#include "ui_AboutDialog.h"

using namespace Ui;

class QDoubleValidator;
class QIntValidator;

class AboutDialog : public QDialog, public AboutDialogClass
{
    Q_OBJECT

public:

    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void showAbout();
    void showPreferences();

    void setCurrentLanguage(const QString &);

public slots:

    void on_pushButtonClearLog_pressed();
    void on_comboBoxLanguage_activated(int index);
    void on_listWidgetTranslations_itemClicked(QListWidgetItem *item);
    void on_comboBoxMetrics_currentIndexChanged(int);
    void on_comboBoxOutputParam_currentIndexChanged(int);
    void on_comboBoxInputParam_currentIndexChanged(int);
    void on_lineEditThreshold_textEdited(const QString &text);
    void on_lineEditInputParam_textEdited(const QString &text);
    void on_pushButtonApplyInputParam_pressed();
    void on_pushButtonApplyImageParam_pressed();
    void on_groupBoxEnableThreshold_toggled(bool on);
    void on_tabWidget_currentChanged(int index);

signals:

    void restart();

private:

    void updateAbout();
    void updateCredits();
    void updateChangelog();
    void fillTextedit(const QString &file, QTextEdit *);
    void updatePrefsLanguages();
    void updateInfosLanguages();
    void updateInfosImageFormats();
    void updateWipeEffects();
    void initMetrics();
    void changePalette(bool modified, QLineEdit *lineEdit, QPushButton *button);
    void setupParam(int index, QComboBox *combo, QLineEdit *lineEdit, QPushButton *button);
    void applyParam(QComboBox *combo, QLineEdit *lineEdit, QPushButton *button);

    QString m_currentLanguage;
    QMap<int, QString> m_descMethodHash;
    QDoubleValidator *m_dValidator;
    QIntValidator *m_iValidator;
};

#endif // _DIALOGABOUT_H_
