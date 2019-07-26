
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

#include "DiffImgWindow.h"
#include "FilesDialog.h"
#include "AppSettings.h"
#include "LogHandler.h"
#include "AboutDialog.h"
#include "MiscFunctions.h"

#include "BaseMetric.h"
#include "MetricsRegistering.h"
#include "FormatsRegistering.h"
#include "MetricsManager.h"
#include "FormatsManager.h"
#include "FilesManager.h"
#include "WipeMethod.h"

#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtCore/QtDebug>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include <QActionGroup>
#include <QFileDialog>
#include <QAction>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QPainter>
#include <QDragEnterEvent>
#include <QButtonGroup>
#include <QMimeData>

#if !defined(WIN32)
#include <unistd.h> // for unlink
#endif

#include <math.h>

#ifndef ABS
#define ABS(x) ( ( (x)>=0 ) ? (x) : ( -(x) ) )
#endif

#ifndef MAX
#define MAX(a,b)  ( ( (a) > (b) ) ? (a) : (b) )
#endif

#ifndef MIN
#define MIN(a,b)  ( ( (a) < (b) ) ? (a) : (b) )
#endif

const int defaultThumbnailSize = 128;
const int updateWaitTime = 200;

// QWT
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

class ChannelCurve : public QwtPlotCurve
{
public:

    ChannelCurve(const QString &title) :
        QwtPlotCurve(title)
    {
#if QT_VERSION >= 0x040000
        setRenderHint(QwtPlotItem::RenderAntialiased);
#endif
    }

    void setColor(const QColor &color)
    {
#if QT_VERSION >= 0x040000
        QColor c = color;
        c.setAlpha(50);

        setPen(c);
        setBrush(c);
#else
        setPen(color);
        setBrush( QBrush(color, Qt::Dense4Pattern) );
#endif
    }
};

DiffImgWindow::DiffImgWindow(QWidget *parent, Qt::WindowFlags flags)
    :   QMainWindow(parent, flags),
    m_displayOverlayDiff(true),
    m_displayOriginalImage(true),
    m_displayModifiedImage(false),
    m_displayDifferenceImage(false),
    m_displayDualPanel(false),
    m_displayHistoZeroValue(false),
    m_displayImageComment(false),
    m_newFile1m_newFile1(true),
    m_newFile2m_newFile1(true),
    m_version(PACKAGE_VERSION),
#ifdef WIN64
    m_appName( PACKAGE_NAME + QString(" (x64)") ),
#else
    m_appName(PACKAGE_NAME),
#endif
    m_firstTime(true),
    m_about(NULL),
    m_logLevel(LogHandler::MSG_DEBUG),
    m_widgetHistoCurveR(NULL),
    m_widgetHistoCurveG(NULL),
    m_widgetHistoCurveB(NULL),
    m_resetConfig(false),
    m_maskOpacity(50),
    m_compareWithThreshold(false),
    m_metricType(0),
    m_thresholdType(0),
    m_diffRes(NULL),
    m_currentDisplayType(TYPE_ORIGINAL),
    m_labelZoom(NULL),
    m_labelPos(NULL),
    m_labelInfos(NULL),
    m_applyGain(false),
    m_applyOffset(false),
    m_currentGain(1.0f),
    m_currentOffset(0.0f),
    m_wipeMethod(WipeMethod::WIPE_HORIZONTAL)
{
    setupUi(this);
    updateUi();
    loadSettings();
}

DiffImgWindow::~DiffImgWindow() 
{
    saveSettings();
    delete m_diffRes;
    MetricsManager::clear();
    FormatsManager::clear();
}

void DiffImgWindow::updateUi()
{
    updateApplicationIdentity();

    // metrics registering
    int nb = MetricsRegistering::registerAll();
    LogHandler::getInstance()->reportDebug( QString("Register of %1 metrics").arg(nb) );

    // weird format registering
    nb = FormatsRegistering::registerAll();
    LogHandler::getInstance()->reportDebug( QString("Register of %1 user formats").arg(nb) );

    m_dfiles = new FilesDialog(this);
    connect( m_dfiles, SIGNAL( accepted () ), this, SLOT( acceptFiles() ) );

    m_actionGroup = new QActionGroup(this);
    m_actionGroup->addAction(actionImage1);
    m_actionGroup->addAction(actionImage2);
    m_actionGroup->addAction(actionDifferenceImage);
    m_actionGroup->setExclusive(true);

    m_about = new AboutDialog(this);
    connect( m_about,SIGNAL( restart() ),this,SLOT( restart() ) );
    connect( m_about, SIGNAL( accepted() ), this, SLOT( setPreferences() ) );

    // create and connect the log handler
    connect( LogHandler::getInstance(),SIGNAL( newMessage(const QString &) ),this,SLOT( printToLog(const QString &) ) );
    

    m_diffRes = new DiffStruct;

    // add some labels to status bar
    m_labelInfos = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(m_labelInfos);

    m_labelPos = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(m_labelPos);

    m_labelZoom = new QLabel(this);
    QMainWindow::statusBar()->addPermanentWidget(m_labelZoom);

    connect( graphicsView1,SIGNAL( mouseMoved(const QPointF &) ),this,SLOT( displayMousePosition(const QPointF &) ) );
    connect( graphicsView1,SIGNAL( mouseMoved(const QPointF &) ),graphicsView2,SLOT( setMarkerPosition(const QPointF &) ) );
    connect( graphicsView1,SIGNAL( scaleChanged(qreal) ),this,SLOT( displayZoomFactor(qreal) ) );

    connect( graphicsView2,SIGNAL( mouseMoved(const QPointF &) ),this,SLOT( displayMousePosition(const QPointF &) ) );
    connect( graphicsView2,SIGNAL( mouseMoved(const QPointF &) ),graphicsView1,SLOT( setMarkerPosition(const QPointF &) ) );
    connect( graphicsView2,SIGNAL( scaleChanged(qreal) ),this,SLOT( displayZoomFactor(qreal) ) );

    connect( graphicsViewWipe,SIGNAL( mouseMoved(const QPointF &) ),this,SLOT( displayMousePosition(const QPointF &) ) );
    //connect( graphicsViewWipe,SIGNAL( mouseMoved(const QPointF &) ),graphicsView1,SLOT( setMarkerPosition(const QPointF &) ) );
    connect( graphicsViewWipe,SIGNAL( scaleChanged(qreal) ),this,SLOT( displayZoomFactor(qreal) ) );

    connect( graphicsView1,SIGNAL( somethingChanged() ),this,SLOT( syncPanels() ) );
    connect( graphicsView2,SIGNAL( somethingChanged() ),this,SLOT( syncPanels() ) );
    connect( graphicsViewWipe,SIGNAL( somethingChanged() ),this,SLOT( syncPanels() ) );

    graphicsView2->setShowOverview(false); // no navigator in view 2
    //connect( graphicsView2,SIGNAL( newZoomFactor(qreal) ),graphicsView1,SLOT( zoom(qreal) ) );

    QButtonGroup *bg = new  QButtonGroup(this);
    bg->setExclusive(true);
    bg->addButton(pushButtonFile1);
    bg->addButton(pushButtonFile2);
    bg->addButton(pushButtonDifference);

    // highlight pressed buttons
    pushButtonFile1->setStyleSheet("QPushButton:checked {  background-color:  #ffff00;}");
    pushButtonFile2->setStyleSheet("QPushButton:checked {  background-color:  #ffff00; }");
    pushButtonDifference->setStyleSheet("QPushButton:checked {  background-color:  #ffff00; }");

    // update tooltip with shortcuts ...
    updateTooltips();

    // update timer for gain/offset
    m_timerUpdate = new QTimer(this);
    connect( m_timerUpdate,SIGNAL( timeout() ),this,SLOT( updateDisplay() ) );
    m_timerUpdate->setSingleShot(true);

    LogHandler::getInstance()->setBufferization(false); // get all stored messages if exists

    // Wipe view
    graphicsViewWipe->setWipeMode(true);
    widgetHisto->setMinimumHeight(50);
}

void DiffImgWindow::updateTooltips()
{
    QList<QAction *> actions = findChildren<QAction *>();

    foreach(QAction * act, actions )
    {
        QString tooltip = act->toolTip();
        QKeySequence keyShortcut = act->shortcut();
        QString strShortcutString = keyShortcut.toString();
        if ( !strShortcutString.isEmpty() )
            act->setToolTip( QString("%1 (%2)").arg(tooltip,strShortcutString) );
    }
}

void DiffImgWindow::updateTitle()
{
    int index1 = FilesManager::getFilelist1().indexOf( QFileInfo(m_file1).fileName() );
    int index2 = FilesManager::getFilelist2().indexOf( QFileInfo(m_file2).fileName() );
    int total1 = FilesManager::getFilelist1().size() - 1;
    int total2 = FilesManager::getFilelist2().size() - 1;
    QString title;
    if (index1 >= 0 && index2 >= 0)
        title = QString(" (%1/%2) [%3] / [%4] (%5/%6)").arg(index1).arg(total1).arg( QFileInfo(m_file1).fileName() ).arg( QFileInfo(m_file2).fileName() ).arg(index2).arg(total2);
    else
        title = "[" + QFileInfo(m_file1).fileName() + "] / [" + QFileInfo(m_file2).fileName() + "]";
    setWindowTitle(m_appName + " " + m_version + ": " + title);
}

void DiffImgWindow::setFiles(const QString &file1, const QString &file2)
{
    m_file1 = file1;
    m_file2 = file2;
    m_firstTime = false; // not open dialog file

    // reinit the dialog
    m_dfiles->lineEditFile1->setText(m_file1);
    m_dfiles->lineEditFile2->setText(m_file2);
}

void DiffImgWindow::on_actionQuit_triggered()
{
    QApplication::exit(0);
}

//-----------------------------------------------------------------------
// Save/Restore parameters functions
//-----------------------------------------------------------------------

void DiffImgWindow::saveSettings()
{
    AppSettings settings;

    settings.beginGroup("MainWindow");

    settings.setValue( "MainWindowState",saveState(0) );

    settings.setValue( "size", size() );
    settings.setValue( "pos", pos() );

    settings.setValue( "fullScreen", isFullScreen() );
    settings.setValue( "maximized", isMaximized ());

    settings.endGroup();

    settings.beginGroup("Application");

    // save last diff images
    if ( !m_file1.isEmpty() )
        settings.setValue("lastDiffImage1", m_file1);
    if ( !m_file2.isEmpty() )
        settings.setValue("lastDiffImage2", m_file2);

    settings.setValue("displayDualPanel", m_displayDualPanel);
    settings.setValue("displayHistoZeroValue", m_displayHistoZeroValue);
    settings.setValue("displayImageComment", m_displayImageComment);

    settings.setValue("currentLanguage", m_currentLanguage);

    settings.setValue("resetConfig", m_resetConfig); // not use by now

    settings.setValue("maskOpacity", m_maskOpacity);

    settings.setValue("compareWithThreshold", m_compareWithThreshold);
    settings.setValue("metricType", m_metricType);
    settings.setValue("thresholdType", m_thresholdType);

    settings.setValue("currentDisplayType", m_currentDisplayType);
    settings.setValue("displayOverlayDiff", m_displayOverlayDiff);

    settings.setValue("wipeMethod", m_wipeMethod);

    settings.endGroup();
}

void DiffImgWindow::loadSettings()
{
    AppSettings settings;

    settings.beginGroup("MainWindow");

    restoreState(settings.value("MainWindowState").toByteArray(), 0);

    resize( settings.value( "size", QSize(800, 800) ).toSize() );
    move( settings.value( "pos", QPoint(200, 200) ).toPoint() );

    // check if restore position isn't outside the current screen definitions (additional screen has been unplugged)
    if (QApplication::screenAt( pos() ) == nullptr)
        move( QPoint(200, 200) );

    bool fullScreen = settings.value("fullScreen",false).toBool();
    if (fullScreen)
        showFullScreen ();

    bool maximized = settings.value("maximized",false).toBool();
    if (maximized)
        showMaximized ();

    settings.endGroup();

    settings.beginGroup("Application");

    m_currentLanguage = settings.value("currentLanguage","auto").toString();

    if ( m_currentLanguage.isEmpty() )
        m_currentLanguage = QLocale::system().name().left(2);

    // load last diff images
    m_file1 = settings.value("lastDiffImage1","").toString();
    m_file2 = settings.value("lastDiffImage2","").toString();

    m_displayDualPanel = settings.value("displayDualPanel",false).toBool();
    actionDualPanel->setChecked(m_displayDualPanel);
    on_actionDualPanel_toggled(m_displayDualPanel);

    m_displayHistoZeroValue = settings.value("displayHistoZeroValue",false).toBool();
    checkBoxShowHistoZero->setChecked(m_displayHistoZeroValue);

    m_displayImageComment = settings.value("displayImageComment",false).toBool();
    actionShowComment->setChecked(m_displayImageComment);
    on_actionShowComment_toggled(m_displayImageComment);

    m_resetConfig = settings.value("resetConfig", false).toBool(); // not use by now

    m_maskOpacity = settings.value("maskOpacity", 50).toInt();
    graphicsView1->setMaskOpacity(m_maskOpacity / 100.0f);
    graphicsView2->setMaskOpacity(m_maskOpacity / 100.0f);

    m_compareWithThreshold = settings.value("compareWithThreshold", false).toBool();
    m_metricType = settings.value("metricType", 0).toInt();
    m_thresholdType = settings.value("thresholdType", 0).toInt();

    m_currentDisplayType = settings.value("currentDisplayType", TYPE_ORIGINAL).toInt();
    if (m_currentDisplayType == TYPE_ORIGINAL)
        actionImage1->activate(QAction::Trigger);
    else if (m_currentDisplayType == TYPE_MODIFIED)
        actionImage2->activate(QAction::Trigger);
    else
        actionDifferenceImage->activate(QAction::Trigger);
    m_displayOverlayDiff = settings.value("displayOverlayDiff", true).toBool();
    actionDifferenceMask->setChecked(m_displayOverlayDiff);

    m_wipeMethod = settings.value("wipeMethod", WipeMethod::WIPE_HORIZONTAL).toInt();
    graphicsViewWipe->setWipeMethod(m_wipeMethod);

    //on_actionDifferenceMask_toggled(m_displayOverlayDiff);

    settings.endGroup();
}

// simple function to detect if all files are known
void DiffImgWindow::load()
{
    // display the selection file dialog if needed
    if (m_file1.isEmpty() || m_file2.isEmpty() || m_firstTime) {
        QTimer::singleShot( 0, this, SLOT( on_actionOpen_triggered() ) );
    } else {
        QTimer::singleShot( 0, this, SLOT( acceptFiles() ) );
    }
}

void DiffImgWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl( QUrl( QString(PACKAGE_ONLINE_HELP_URL) ) );
}

void DiffImgWindow::on_actionShowComment_toggled(bool val)
{
    m_displayImageComment = val;
    updateImageComment();
}

void DiffImgWindow::updateImageComment()
{
    if (!m_displayDualPanel)
    {
        lineEditComment1->setVisible(m_displayImageComment);
        lineEditComment1->setEnabled(m_displayImageComment);
        pushButtonClearComment1->setVisible(m_displayImageComment);
        pushButtonValidComment1->setVisible(m_displayImageComment);

        // always hidden
        lineEditComment2->setVisible(false);
        pushButtonClearComment2->setVisible(false);
        pushButtonValidComment2->setVisible(false);
        pushButtonValidComment2->setEnabled(false);
    }
    else
    {
        lineEditComment1->setVisible(m_displayImageComment);
        lineEditComment1->setEnabled(m_displayImageComment);
        pushButtonClearComment1->setVisible(m_displayImageComment);
        pushButtonValidComment1->setVisible(m_displayImageComment);
        lineEditComment2->setVisible(m_displayImageComment);
        lineEditComment2->setEnabled(m_displayImageComment);
        pushButtonClearComment2->setVisible(m_displayImageComment);
        pushButtonValidComment2->setVisible(m_displayImageComment);
    }
}

void DiffImgWindow::on_pushButtonClearComment1_pressed()
{
    lineEditComment1->clear();
}

void DiffImgWindow::on_pushButtonClearComment2_pressed()
{
    lineEditComment2->clear();
}

QString DiffImgWindow::readComment(const QString &imageFile)
{
    QString commentFile = imageFile + ".txt";
    return MiscFunctions::fileToString(commentFile);
}

void DiffImgWindow::writeComment(const QString &comment, const QString &imageFile)
{
    QString commentFile = imageFile + ".txt";

    // delete file if needed
    if ( comment.trimmed().isEmpty() && QFileInfo(commentFile).exists() )
    {
        unlink( commentFile.toStdString().c_str() );
    }
    else if ( !MiscFunctions::stringToFile(comment.trimmed(),commentFile) )
    {
        QMessageBox::warning( this, PACKAGE_NAME,tr("Can't save comment data for %1").arg(imageFile) );
    }
}

void DiffImgWindow::on_actionSaveDifference_triggered()
{
    // ask for a filename ...
    QString savefilename = QFileDialog::getSaveFileName( this, tr("Save difference image"),
                                                         "diff.png",
                                                         MiscFunctions::getAvailablesImageFormats() );

    if ( !savefilename.isEmpty() )
    {
        BaseMetric *met = MetricsManager::getMetrics()[m_metricType];
        if ( !met->saveDifference(savefilename) )
            LogHandler::getInstance()->reportError( QString("Can't save difference image in %1").arg(savefilename) );
    }
}

void DiffImgWindow::on_actionOpen_triggered()
{
    // reinit
    m_dfiles->setFile1(m_file1);
    m_dfiles->setFile2(m_file2);
    m_dfiles->show();
}

void DiffImgWindow::on_actionNext_triggered()
{
    QString file1 = m_file1, file2 = m_file2;
    FilesManager::getNextFiles(file1,file2);

    if ( file1.isEmpty() || file2.isEmpty() )
        return;

    m_dfiles->setFile1(file1);
    m_dfiles->setFile2(file2);

    acceptFiles();
}

void DiffImgWindow::on_actionPrev_triggered()
{
    QString file1 = m_file1, file2 = m_file2;
    FilesManager::getPrevFiles(file1,file2);

    if ( file1.isEmpty() || file2.isEmpty() )
        return;

    m_dfiles->setFile1(file1);
    m_dfiles->setFile2(file2);

    acceptFiles();
}

void DiffImgWindow::computeDifferenceNew()
{
    BaseMetric *met = MetricsManager::getMetrics()[m_metricType];

    // TODO put htis at the right place !!!
    labelCurrentMetric->setText( tr("Current metric: %1").arg( met->getName() ) );

    met->checkDifferences(m_file1,m_file2);

    if ( !met->isValid() )
    {
        QMessageBox::information( this, PACKAGE_NAME,tr("Can't display difference, more information in the log panel ...") );
        return;
    }

    updateImage1( met->getImage1() );
    updateImage2( met->getImage2() );
    updateDifference( met->getImageDifference() );
    m_maskDiffImage = met->getImageMask();
    graphicsView1->setMask(m_maskDiffImage);
    graphicsView2->setMask(m_maskDiffImage);
    on_actionFitToWindow_triggered();

    widgetProperties->displayData(met);

    updateSmileyStatus();
    showRGBHistogramNew();
    updateDisplay();
}

void DiffImgWindow::acceptFiles()
{
    m_dfiles->getCurrentFiles(m_file1,m_file2);

    updateTitle();
    computeDifferenceNew();
}

void DiffImgWindow::on_actionDisplayWipe_toggled(bool val)
{
    actionDualPanel->setEnabled(!val);
    m_actionGroup->setEnabled(!val);
    pushButtonFile1->setEnabled(!val);
    pushButtonFile2->setEnabled(!val);
    pushButtonDifference->setEnabled(!val);
    //dockWidgetNavigator->setEnabled(!val);
    if (val)
    {
        stackedWidget->setCurrentIndex(PAGE_WIPEVIEW);
    }
    else
    {
        stackedWidget->setCurrentIndex(PAGE_DUALVIEW);
    }
}

void DiffImgWindow::on_actionDualPanel_toggled(bool val)
{
    m_displayDualPanel = val;
    setModeDualPanel(m_displayDualPanel);

    // enable/disable some actions
    actionImage2->setEnabled(!m_displayDualPanel);

    // if dual panel mode => force original image display
    if (m_displayDualPanel)
        actionImage1->trigger();
}

//-------------------------------------------------------------------------

void DiffImgWindow::updateSmileyStatus()
{
    BaseMetric *met = MetricsManager::getMetrics()[m_metricType];

    QPixmap pixmap;
    // setup the emoticon
    if ( !met->getPixelError() ) {
        pixmap = QPixmap(":/diffimg/allgood.png");
    } else if (m_compareWithThreshold && met->selectedStatsIsValid()) {
        pixmap = QPixmap(":/diffimg/somebad.png");
    } else {
        pixmap = QPixmap(":/diffimg/bad.png");
    }
    labelSmiley->setPixmap(pixmap.scaledToHeight(labelSmiley->height()));
}

//-------------------------------------------------------------------------

void DiffImgWindow::openFile1(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return;

    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::information( this, tr("Image Viewer"),tr("Cannot load %1.").arg(fileName) );
        return;
    }
    updateImage1(image);
}

void DiffImgWindow::updateImage1(const QImage &image)
{
    graphicsView1->filenameLeft = QFileInfo(m_file1).fileName();
    graphicsViewWipe->filenameLeft = QFileInfo(m_file1).fileName();

    m_image1 = image;
    m_image1Thumbnail = m_image1.scaled( pushButtonFile1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation );

    //m_resultImage1 = QImage(m_image1.size(), QImage::Format_ARGB32);

    // set default iconsize for pushbutton
    pushButtonFile1->setIconSize( m_image1Thumbnail.size() );
    pushButtonFile1->setIcon( QPixmap::fromImage(m_image1Thumbnail) );

    lineEditComment1->setText( readComment(m_file1) );
    pushButtonValidComment1->setEnabled(false);

    //labelSize->setText( QString("%1x%2").arg( m_sourceImage.size().width() ).arg( m_sourceImage.size().height() ) );

    // init of the
//    scrollAreaImage1->setImage(m_sourceImage);
    graphicsView1->setImage(m_image1);
    graphicsViewWipe->setWipeImage1(m_image1);

    // m_size = m_image1.size();
    on_actionFitToWindow_triggered();
}

void DiffImgWindow::setPanel1Visibility(bool val)
{
    graphicsView1->setVisible(val);
    lineEditComment1->setVisible(val&&m_displayImageComment);
    pushButtonClearComment1->setVisible(val&&m_displayImageComment);
    pushButtonValidComment1->setVisible(val&&m_displayImageComment);
}

void DiffImgWindow::setPanel2Visibility(bool val)
{
    graphicsView2->setVisible(val);
    lineEditComment2->setVisible(val&&m_displayImageComment);
    pushButtonClearComment2->setVisible(val&&m_displayImageComment);
    pushButtonValidComment2->setVisible(val&&m_displayImageComment);
}

void DiffImgWindow::updateDisplay()
{
    BaseMetric *met = MetricsManager::getMetrics()[m_metricType];

    // always update image2
    QImage img2;
    if (m_applyGain || m_applyOffset)
        img2 = met->getImage2WithGain(m_currentGain,m_currentOffset);
    else
        img2 = m_image2;
    graphicsView2->setImage(img2);

    setPanel1Visibility(true);

    // new mode
    if (stackedWidget->currentIndex() == PAGE_WIPEVIEW) {
        QImage img1;
        if (m_applyGain || m_applyOffset) {
            graphicsViewWipe->setWipeImage1(met->getImage1WithGain(m_currentGain,m_currentOffset));
            graphicsViewWipe->setWipeImage2(met->getImage2WithGain(m_currentGain,m_currentOffset));
        } else {
            graphicsViewWipe->setWipeImage1(m_image1);
            graphicsViewWipe->setWipeImage2(m_image2);
        }
    } else if (m_displayDualPanel) {
        setPanel2Visibility(true);
        graphicsView2->setShowOverview(false);
        if (m_currentDisplayType == TYPE_ORIGINAL)
        {
            QImage img1;
            if (m_applyGain || m_applyOffset)
                img1 = met->getImage1WithGain(m_currentGain,m_currentOffset);
            else
                img1 = m_image1;
            graphicsView1->setImage(img1);
        }
        else
        {
            QImage img1;
            if (m_applyGain || m_applyOffset)
                img1 = met->getImageDifferenceWithGain(m_currentGain,m_currentOffset);
            else
                img1 = m_diffImage;
            graphicsView1->setImage(img1);
        }
    } else {
        setPanel2Visibility(false);
        graphicsView2->setShowOverview(false);
        QImage img1;
        if (m_currentDisplayType == TYPE_ORIGINAL)
        {
            if (m_applyGain || m_applyOffset)
                img1 = met->getImage1WithGain(m_currentGain,m_currentOffset);
            else
                img1 = m_image1;
        }
        else if (m_currentDisplayType == TYPE_MODIFIED)
        {
            if (m_applyGain || m_applyOffset)
                img1 = met->getImage2WithGain(m_currentGain,m_currentOffset);
            else
                img1 = m_image2;
        }
        else
        {
            if (m_applyGain || m_applyOffset)
                img1 = met->getImageDifferenceWithGain(m_currentGain,m_currentOffset);
            else
                img1 = m_diffImage;
        }
        graphicsView1->setImage(img1);
    }
}

//-------------------------------------------------------------------------

void DiffImgWindow::openFile2(const QString &fileName)
{
    if (fileName.isEmpty()) {
        return;
    }

    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::information( this, tr("Image Viewer"),tr("Cannot load %1.").arg(fileName) );
        return;
    }

    updateImage2(image);
}

void DiffImgWindow::updateImage2(const QImage &image)
{
    graphicsView2->filenameRight = QFileInfo(m_file2).fileName();
    graphicsViewWipe->filenameRight = QFileInfo(m_file2).fileName();

    m_image2 = image;
    m_image2Thumbnail = m_image2.scaled( pushButtonFile2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation );

    // set default iconsize for pushbutton
    pushButtonFile2->setIconSize( m_image2Thumbnail.size() );
    pushButtonFile2->setIcon( QPixmap::fromImage(m_image2Thumbnail) );

    lineEditComment2->setText( readComment(m_file2) );
    pushButtonValidComment2->setEnabled(false);

    graphicsView2->setImage(m_image2);
    graphicsViewWipe->setWipeImage2(m_image2);
    if (m_displayDualPanel)
        synchronizePanels();
}

void DiffImgWindow::updateDifference(const QImage &image)
{
    m_diffImage = image;
    m_diffImageThumbnail = m_diffImage.scaled( pushButtonDifference->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
    pushButtonDifference->setIconSize( m_diffImageThumbnail.size() );
    pushButtonDifference->setIcon( QPixmap::fromImage(m_diffImageThumbnail) );
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_actionDifferenceMask_toggled(bool val)
{
    m_displayOverlayDiff = val;
    graphicsView1->setEnabledMask(m_displayOverlayDiff);
    graphicsView2->setEnabledMask(m_displayOverlayDiff);

    updateDisplay();
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_actionImage1_toggled(bool val)
{
    m_displayOriginalImage = val;
    if (m_displayOriginalImage)
    {
        m_currentDisplayType = TYPE_ORIGINAL;
        showRGBHistogramNew();
        m_displayModifiedImage = false;
        m_displayDifferenceImage = false;

        if ( !pushButtonFile1->isChecked() )
            pushButtonFile1->setChecked(true);

        graphicsView1->setScale( graphicsView2->getScale() );
        graphicsView1->setCenter( graphicsView2->getCenter() );

        lineEditComment1->setText( readComment(m_file1) );
        pushButtonValidComment1->setEnabled(false);
        lineEditComment1->setEnabled(true);
        pushButtonClearComment1->setEnabled(true);
        pushButtonValidComment1->setEnabled(true);

        updateDisplay();
        checkBoxShowHistoZero->setVisible(false);
        labelHist->setVisible(false);
    }
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_actionImage2_toggled(bool val)
{
    m_displayModifiedImage = val;
    if (m_displayModifiedImage)
    {
        m_currentDisplayType = TYPE_MODIFIED;
        showRGBHistogramNew();
        m_displayOriginalImage = false;
        m_displayDifferenceImage = false;

        if ( !pushButtonFile2->isChecked() )
            pushButtonFile2->setChecked(true);

        graphicsView2->setScale( graphicsView1->getScale() );
        graphicsView2->setCenter( graphicsView1->getCenter() );

        lineEditComment1->setText( readComment(m_file2) );
        pushButtonValidComment1->setEnabled(false);
        lineEditComment1->setEnabled(true);
        pushButtonClearComment1->setEnabled(true);
        pushButtonValidComment1->setEnabled(true);

        updateDisplay();
        checkBoxShowHistoZero->setVisible(false);
        labelHist->setVisible(false);
    }
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_actionDifferenceImage_toggled(bool val)
{
    m_displayDifferenceImage = val;
    if (m_displayDifferenceImage)
    {
        m_currentDisplayType = TYPE_DIFFERENCE;
        showRGBHistogramNew();
        m_displayOriginalImage = false;
        m_displayModifiedImage = false;
        if ( !pushButtonDifference->isChecked() )
            pushButtonDifference->setChecked(true);

        lineEditComment1->setText("");
        pushButtonValidComment1->setEnabled(false);
        lineEditComment1->setEnabled(false);
        pushButtonClearComment1->setEnabled(false);
        pushButtonValidComment1->setEnabled(false);

        updateDisplay();
    }
    checkBoxShowHistoZero->setVisible(val);
    labelHist->setVisible(val);
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_actionFitToWindow_triggered()
{
    graphicsView1->fitScale();
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_actionFullResolution_triggered()
{
    graphicsView1->resetScale();
    graphicsView2->resetScale();
    graphicsViewWipe->resetScale();
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_actionShowDocks_triggered()
{
    dockWidgetNavigator->show();
    dockWidgetProperties->show();
    widgetHisto->setMinimumHeight(50);
    dockWidgetHistogram->show();
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_pushButtonFile1_pressed()
{
    actionImage1->setChecked(true);
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_pushButtonFile2_pressed()
{
    if (m_displayDualPanel) {
        return;
    }

    actionImage2->setChecked(true);
}

//-------------------------------------------------------------------------

void DiffImgWindow::on_pushButtonDifference_pressed()
{
    actionDifferenceImage->setChecked(true);
}

//-------------------------------------------------------------------------

void DiffImgWindow::resetView()
{
    pushButtonFile1->setIcon( QIcon(":/diffimg/original.png") );
    pushButtonFile2->setIcon( QIcon(":/diffimg/modified.png") );
    pushButtonDifference->setIcon( QIcon(":/diffimg/diffimg.png") );
}

//-------------------------------------------------------------------------

void DiffImgWindow::setModeSingleImage()
{
    pushButtonFile2->setCheckable(false);
    pushButtonDifference->setCheckable(false);
}

//-------------------------------------------------------------------------

void DiffImgWindow::setModeDualImage()
{
    pushButtonFile2->setCheckable(true);
    pushButtonDifference->setCheckable(true);
}

//-------------------------------------------------------------------------

void DiffImgWindow::setModeDualPanel(bool val)
{
    m_displayDualPanel = val;
    if (m_displayDualPanel)
    {
        // for a problem of incorrect layout (??), I have to hide both view before showing them
        setPanel2Visibility(false);
        setPanel1Visibility(false);

        //graphicsView2->hide();
        //graphicsView1->hide();

        setPanel2Visibility(true);
        setPanel1Visibility(true);

        //graphicsView2->show();
        //graphicsView1->show();

        pushButtonFile2->setCheckable(false);
    }
    else
    {
        setPanel2Visibility(false);
        pushButtonFile2->setCheckable(true);
    }
    synchronizePanels();
}

void DiffImgWindow::etHop()
{
    graphicsView2->parentWidget()->layout()->invalidate();
}

//-------------------------------------------------------------------------

void DiffImgWindow::syncPanels()
{
    ImageView *iv = qobject_cast<ImageView *>( sender() );
    if (iv == graphicsView1)
    {
        graphicsView2->setScale( graphicsView1->getScale() );
        graphicsView2->setCenter( graphicsView1->getCenter() );
        graphicsViewWipe->setScale( graphicsView1->getScale() );
        graphicsViewWipe->setCenter( graphicsView1->getCenter() );
    }
    else if (iv == graphicsView2)
    {
        graphicsView1->setScale( graphicsView2->getScale() );
        graphicsView1->setCenter( graphicsView2->getCenter() );
        graphicsViewWipe->setScale( graphicsView2->getScale() );
        graphicsViewWipe->setCenter( graphicsView2->getCenter() );
    }
    else if (iv == graphicsViewWipe)
    {
        graphicsView1->setScale( graphicsViewWipe->getScale() );
        graphicsView1->setCenter( graphicsViewWipe->getCenter() );
        graphicsView2->setScale( graphicsViewWipe->getScale() );
        graphicsView2->setCenter( graphicsViewWipe->getCenter() );
    }
}

//-------------------------------------------------------------------------

void DiffImgWindow::synchronizePanels()
{
    // reference is panel1
    graphicsView2->setScale( graphicsView1->getScale() );
}

void DiffImgWindow::updateApplicationIdentity()
{
    setWindowTitle(m_appName + " " + m_version);
    QApplication::setApplicationName(m_appName);
    QApplication::setApplicationVersion(m_version);
    QApplication::setOrganizationName(PACKAGE_ORGANIZATION);
}

void DiffImgWindow::on_actionAbout_triggered()
{
    updateAbout();

    m_about->show();
    m_about->showAbout();
}

void DiffImgWindow::on_actionPreferences_triggered()
{
    updateAbout();

    m_about->show();
    m_about->showPreferences();
}

void DiffImgWindow::updateAbout()
{
    // update prefs if needed
    m_about->horizontalSliderOpacity->setValue(m_maskOpacity);
    m_about->setCurrentLanguage(m_currentLanguage);

    m_about->groupBoxEnableThreshold->setChecked(m_compareWithThreshold);
    m_about->comboBoxMetrics->setCurrentIndex(m_metricType);
    m_about->comboBoxOutputParam->setCurrentIndex(m_thresholdType);

    m_about->comboBoxWipeEffectAxis->setCurrentIndex(m_wipeMethod);
}

//--------------------------------------------------------------------------------------
// LOG Functions
//--------------------------------------------------------------------------------------

void DiffImgWindow::printToLog(const QString & mess)
{
    m_about->textBrowserLog->append(mess);
}

//---------------------------------------------------------------------------------------------------------------------------------

void DiffImgWindow::initHistoNew()
{
    widgetHisto->detachItems();

    BaseMetric *met = MetricsManager::getMetrics()[m_metricType];

    // histogram
    //widgetHisto->setAxisScale(QwtPlot::xBottom,met->getMinError(),met->getMaxError());
    widgetHisto->setCanvasBackground (Qt::white);
    int nbChannels = 0;

    if (m_currentDisplayType == TYPE_ORIGINAL)
    {
        nbChannels = met->getImage1Channels();
        widgetHisto->setAxisScale( QwtPlot::xBottom,met->getMinImage1(),met->getMaxImage1() );
    }
    else if (m_currentDisplayType == TYPE_MODIFIED)
    {
        nbChannels = met->getImage2Channels();
        widgetHisto->setAxisScale( QwtPlot::xBottom,met->getMinImage2(),met->getMaxImage2() );
    }
    else
    {
        nbChannels = met->getDifferenceChannels();
        widgetHisto->setAxisScale( QwtPlot::xBottom,met->getMinError(),met->getMaxError() );
    }

    if (nbChannels == 1)
    {
        // prepare RGB curves
        m_widgetHistoCurveR = new ChannelCurve("Value");
        m_widgetHistoCurveR->setColor(Qt::black);
    }
    else
    {
        // prepare RGB curves
        m_widgetHistoCurveR = new ChannelCurve("Red");
        m_widgetHistoCurveR->setColor(Qt::red);

        m_widgetHistoCurveG = new ChannelCurve("Green");
        m_widgetHistoCurveG->setColor(Qt::green);

        m_widgetHistoCurveB = new ChannelCurve("Blue");
        m_widgetHistoCurveB->setColor(Qt::blue);
    }
}

void DiffImgWindow::showRGBHistogramNew()
{
    initHistoNew();
    applyHisto();
}

void DiffImgWindow::applyHisto()
{
    BaseMetric *met = MetricsManager::getMetrics()[m_metricType];
    const QList<QPolygonF> * pHistos = NULL;

    if (m_currentDisplayType == TYPE_ORIGINAL)
        pHistos = &met->getHistogramImage1();
    else if (m_currentDisplayType == TYPE_MODIFIED)
        pHistos = &met->getHistogramImage2();
    else
        pHistos = &met->getHistogramImageDiff(m_displayHistoZeroValue);

    for (int i = 0; i < (*pHistos).size(); i++)
    {
        if ( (*pHistos)[i].isEmpty() )
            continue;

        if (i==0)
        {
            m_widgetHistoCurveR->setSamples( (*pHistos)[i] );
            m_widgetHistoCurveR->attach(widgetHisto);
        }
        else if (i == 1)
        {
            m_widgetHistoCurveR->setSamples( (*pHistos)[i] );
            m_widgetHistoCurveR->attach(widgetHisto);
        }
        else if (i == 2)
        {
            m_widgetHistoCurveB->setSamples( (*pHistos)[i] );
            m_widgetHistoCurveB->attach(widgetHisto);
        }
    }

    // plot the histogram
    widgetHisto->replot();
    widgetHisto->setMinimumHeight(50);
}

void DiffImgWindow::restart()
{
    QProcess::startDetached( qApp->arguments()[0], qApp->arguments().mid(1) );
    QApplication::exit(0);
}

//------------------------------------------------------------------------------------------
// preferences functions
//------------------------------------------------------------------------------------------

void DiffImgWindow::setPreferences()
{
    if (!m_about)
        return;

    // get the language
    m_currentLanguage = m_about->comboBoxLanguage->itemData( m_about->comboBoxLanguage->currentIndex() ).toString();

    // get the diff method

    bool hasDifference = false;

    hasDifference |= ( m_maskOpacity != m_about->horizontalSliderOpacity->value() );

    hasDifference |= ( m_compareWithThreshold != m_about->groupBoxEnableThreshold->isChecked() );
    hasDifference |= ( m_metricType != m_about->comboBoxMetrics->currentIndex() );
    hasDifference |= ( m_thresholdType != m_about->comboBoxOutputParam->currentIndex() );

    if (hasDifference)
    {
        m_maskOpacity = m_about->horizontalSliderOpacity->value();
        graphicsView1->setMaskOpacity(m_maskOpacity / 100.0f);
        graphicsView2->setMaskOpacity(m_maskOpacity / 100.0f);

        m_compareWithThreshold = m_about->groupBoxEnableThreshold->isChecked();
        m_metricType = m_about->comboBoxMetrics->currentIndex();
        m_thresholdType = m_about->comboBoxOutputParam->currentIndex();

        computeDifferenceNew();
    }

    if (m_wipeMethod != m_about->comboBoxWipeEffectAxis->currentIndex())
    {
        m_wipeMethod = m_about->comboBoxWipeEffectAxis->currentIndex();
        graphicsViewWipe->setWipeMethod(m_wipeMethod);
    }

    saveSettings();
}

void DiffImgWindow::on_actionRefresh_triggered()
{
    acceptFiles();
}

void DiffImgWindow::displayMousePosition(const QPointF &p)
{
    QPoint pt( (int)p.x(),(int)p.y() );
    if ( pt.isNull() )
        m_labelPos->setText("");
    else
        m_labelPos->setText( QString("%1x%2").arg( pt.x() ).arg( pt.y() ) );

    // display a marker if needed
    ImageView *view = qobject_cast<ImageView *>( sender() );
    if (!view)
        return;

    graphicsView1->setEnabledMarker(view != graphicsView1);
    graphicsView2->setEnabledMarker(view != graphicsView2);

    // display pixel infos
    QString infos;
    if ( !pt.isNull() )
    {
        BaseMetric *met = MetricsManager::getMetrics()[m_metricType];
        if (m_displayDualPanel)
        {
            if (m_currentDisplayType == TYPE_ORIGINAL)
            {
                infos += met->getImage1Data(pt);
                infos += " | ";
                infos += met->getImage2Data(pt);
            }
            else
            {
                infos += met->getErrorData(pt);
                infos += " | ";
                infos += met->getImage2Data(pt);
            }
        }
        else
        {
            if (m_currentDisplayType == TYPE_ORIGINAL)
            {
                infos += met->getImage1Data(pt);
            }
            else if (m_currentDisplayType == TYPE_MODIFIED)
            {
                infos += met->getImage2Data(pt);
            }
            else
            {
                infos += met->getErrorData(pt);
            }
        }
    }
    m_labelInfos->setText(infos);
}

void DiffImgWindow::displayZoomFactor(qreal zoom)
{
    m_labelZoom->setText( tr(" Scale x%1 ").arg( zoom, 3, 'g', 3 ) );
}

void DiffImgWindow::on_sliderGain_valueChanged(int val)
{
    m_currentGain = val / 100.0f;
    doubleSpinBoxGain->setValue( (double)m_currentGain );
    if (m_timerUpdate->remainingTime() < updateWaitTime / 10) {
        m_timerUpdate->start(updateWaitTime);
    }
}

void DiffImgWindow::on_doubleSpinBoxGain_valueChanged(double val)
{
    m_currentGain = val;
    sliderGain->setValue( (int)(val * 100) );
}

void DiffImgWindow::on_sliderOffset_valueChanged(int val)
{
    m_currentOffset = val;
    doubleSpinBoxOffset->setValue( (double)m_currentOffset );
    if (m_timerUpdate->remainingTime() < updateWaitTime / 10) {
        m_timerUpdate->start(updateWaitTime);
    }

//     if (m_lyr)
//         m_lyr->setOffset(currentOffset_);
//
//     displayImage();
}

void DiffImgWindow::on_doubleSpinBoxOffset_valueChanged(double val)
{
    m_currentOffset = val;
    sliderOffset->setValue( (int)(val) );
}

void DiffImgWindow::on_checkBoxGain_toggled(bool val)
{
    m_applyGain = val;
    if (m_applyGain)
    {
        on_sliderGain_valueChanged( sliderGain->value() );
    }
    else
    {
        m_currentGain = 1.0f;
        updateDisplay();
    }
}

void DiffImgWindow::on_checkBoxOffset_toggled(bool val)
{
    m_applyOffset = val;
    if (m_applyOffset)
    {
        on_sliderOffset_valueChanged( sliderOffset->value() );
    }
    else
    {
        m_currentOffset = 0.0;
        updateDisplay();
    }
}

// ------------- Drag and drop -----------

void DiffImgWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if ( !mimeData->hasUrls() )
    {
        event->ignore();
        return;
    }

    QList<QUrl> urls = mimeData->urls();

    if(urls.count() != 2)
    {
        event->ignore();
        return;
    }

    QUrl url = urls.at(0);
    QString filename = url.toLocalFile();

    // We don't test extension
    if ( !QFileInfo(filename).exists() )
    {
        event->ignore();
        return;
    }

    if ( !QFileInfo(filename).isFile() )
    {
        event->ignore();
        return;
    }

    event->acceptProposedAction();
}

void DiffImgWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if ( event->source() == this )
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->acceptProposedAction();
    }
}

void DiffImgWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    QList<QUrl> urls = mimeData->urls();

    if (urls.size()>1)
    {
        QString file1 = urls.at(0).toLocalFile();
        QString file2 = urls.at(1).toLocalFile();
        m_dfiles->setFile1(file1);
        m_dfiles->setFile2(file2);
        acceptFiles();
    }
}

void DiffImgWindow::on_checkBoxShowHistoZero_toggled(bool val)
{
    m_displayHistoZeroValue = val;
    showRGBHistogramNew();
}

void DiffImgWindow::on_lineEditComment1_textChanged(const QString &)
{
    pushButtonValidComment1->setEnabled(true);
}

void DiffImgWindow::on_lineEditComment2_textChanged(const QString &)
{
    pushButtonValidComment2->setEnabled(true);
}

void DiffImgWindow::on_lineEditComment1_returnPressed()
{
    pushButtonValidComment1->setEnabled(false);
    writeComment(lineEditComment1->text(),m_file1);
}

void DiffImgWindow::on_lineEditComment2_returnPressed()
{
    pushButtonValidComment2->setEnabled(false);
    writeComment(lineEditComment2->text(),m_file2);
}

void DiffImgWindow::on_pushButtonValidComment1_pressed()
{
    on_lineEditComment1_returnPressed();
}

void DiffImgWindow::on_pushButtonValidComment2_pressed()
{
    on_lineEditComment2_returnPressed();
}

void DiffImgWindow::keyPressEvent ( QKeyEvent * event )
{
    bool ctrl = event->modifiers() & Qt::ControlModifier;

    if (ctrl)
    {
        if (event->key() == Qt::Key_Plus)
        {
            graphicsView1->zoomIn();
        }
        else if (event->key() == Qt::Key_Minus)
        {
            graphicsView1->zoomOut();
        }
    }
}




