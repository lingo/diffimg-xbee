
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

#ifndef _DIFFIMGWINDOW_H_
#define _DIFFIMGWINDOW_H_

#include <QMainWindow>
#include "ui_DiffImgWindow.h"

// forward declarations
class FilesDialog;
class AboutDialog;
class ChannelCurve;
class QLabel;
class QTimer;
class QActionGroup;

struct DiffStruct;

class DiffImgWindow : public QMainWindow, private Ui::DiffImgWindowClass
{
    Q_OBJECT

public:



    DiffImgWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~DiffImgWindow();

public slots:

    void setFiles(const QString &original, const QString &modified);
    void load();

    void on_actionDifferenceMask_toggled(bool val);
    void on_actionImage1_toggled(bool val);
    void on_actionImage2_toggled(bool val);
    void on_actionDifferenceImage_toggled(bool val);

    void on_actionQuit_triggered();
    void on_actionFitToWindow_triggered();
    void on_actionFullResolution_triggered();

    void on_actionRefresh_triggered();

    void on_actionNext_triggered();
    void on_actionPrev_triggered();

    void on_actionOpen_triggered();
    void on_actionHelp_triggered();
    void on_actionSaveDifference_triggered();
    void on_actionShowComment_toggled(bool);

    void on_actionShowDocks_triggered();

    void on_actionDualPanel_toggled(bool val);
    void on_actionDisplayWipe_toggled(bool val);

    void openFile1(const QString &fileName);
    void openFile2(const QString &fileName);

    void computeDifferenceNew();

    void on_pushButtonFile1_pressed();
    void on_pushButtonFile2_pressed();
    void on_pushButtonDifference_pressed();

    void resetView();
    void setModeSingleImage();
    void setModeDualImage();

    void setModeDualPanel(bool val);

    void synchronizePanels();
    void syncPanels();

    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();

    void printToLog(const QString &mess);

    void restart();
    void setPreferences();

    void on_checkBoxShowHistoZero_toggled(bool);

    void setPanel1Visibility(bool);
    void setPanel2Visibility(bool);

protected slots:

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event) override;

private slots:

    void acceptFiles();
    void displayZoomFactor(qreal zoom);
    void displayMousePosition(const QPointF &pt);
    void etHop();

    void on_sliderGain_valueChanged(int);
    void on_checkBoxGain_toggled(bool);
    void on_doubleSpinBoxGain_valueChanged(double);
    void on_sliderOffset_valueChanged(int);
    void on_checkBoxOffset_toggled(bool);
    void on_doubleSpinBoxOffset_valueChanged(double);

    void on_pushButtonClearComment1_pressed();
    void on_pushButtonClearComment2_pressed();
    void on_pushButtonValidComment1_pressed();
    void on_pushButtonValidComment2_pressed();

    void on_lineEditComment1_textChanged(const QString &);
    void on_lineEditComment2_textChanged(const QString &);
    void on_lineEditComment1_returnPressed();
    void on_lineEditComment2_returnPressed();

    void updateDisplay();

private:

    void updateUi();
    void updateAbout();

    void saveSettings();
    void loadSettings();

    void updateTitle();
    void updateTooltips();

    void updateApplicationIdentity();
    void initHistoNew();
    void applyHisto();
    void showRGBHistogramNew();

    void updateImage1(const QImage &image);
    void updateImage2(const QImage &image);
    void updateDifference(const QImage &image);

    void updateSmileyStatus();
    QString readComment(const QString &imageFile);
    void writeComment(const QString &comment, const QString &imageFile);
    void updateImageComment();

    enum typImage {
        TYPE_ORIGINAL = 0,
        TYPE_MODIFIED,
        TYPE_DIFFERENCE
    };

    enum typPage {
        PAGE_DUALVIEW = 0,
        PAGE_WIPEVIEW
    };

    bool m_displayOverlayDiff;
    bool m_displayOriginalImage;
    bool m_displayModifiedImage;
    bool m_displayDifferenceImage;
    bool m_displayDualPanel;
    bool m_displayHistoZeroValue;
    bool m_displayImageComment;

    QString m_file1;
    QStringList m_lFiles1Deprecated;
    QString m_file1pathDeprecated;
    bool m_newFile1m_newFile1;

    QString m_file2;
    QStringList m_lFiles2m_newFile1;
    QString m_file2pathm_newFile1;
    bool m_newFile2m_newFile1;

    FilesDialog *m_dfiles;

    QString m_version;
    QString m_appName;
    bool m_firstTime;

    QImage m_image1;
    QImage m_image1Thumbnail;
    QImage m_image2;
    QImage m_image2Thumbnail;
    QImage m_diffImage;
    QImage m_diffImageThumbnail;

    QImage m_maskDiffImage;

    QString m_currentLanguage;

    AboutDialog *m_about;
    int m_logLevel;

    ChannelCurve *m_widgetHistoCurveR;
    ChannelCurve *m_widgetHistoCurveG;
    ChannelCurve *m_widgetHistoCurveB;

    bool m_resetConfig;
    int m_maskOpacity;

    bool m_compareWithThreshold;
    int m_metricType;
    int m_thresholdType;

    DiffStruct *m_diffRes;
    int m_currentDisplayType;
    QLabel *m_labelZoom;
    QLabel *m_labelPos;
    QLabel *m_labelInfos;
    bool m_applyGain;
    bool m_applyOffset;
    qreal m_currentGain;
    qreal m_currentOffset;

    QTimer *m_timerUpdate;
    QActionGroup *m_actionGroup;

    int m_wipeMethod;
};

#endif // _DIFFIMGWINDOW_H_
