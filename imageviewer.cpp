/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "imageviewer.h"

#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#include <QString>
#endif
#endif

ImageViewer::ImageViewer(QWidget *parent)
   : QMainWindow(parent), imageLabelRight(new clickimagelabel), imageLabelLeft(new clickimagelabel),
     scrollAreaRight(new QScrollArea), scrollAreaLeft(new QScrollArea), scaleFactor(1)
{
    // Left image
    imageLabelLeft = new clickimagelabel;
    imageLabelLeft->setName("Left");
    imageLabelLeft->setBackgroundRole(QPalette::Base);
    imageLabelLeft->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabelLeft->setScaledContents(true);

    scrollAreaLeft = new QScrollArea;
    scrollAreaLeft->setBackgroundRole(QPalette::Dark);
    scrollAreaLeft->setWidget(imageLabelLeft);

    // Right image
    imageLabelRight = new clickimagelabel;
    imageLabelRight->setName("Right");
    imageLabelRight->setBackgroundRole(QPalette::Base);
    imageLabelRight->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabelRight->setScaledContents(true);

    scrollAreaRight = new QScrollArea;
    scrollAreaRight->setBackgroundRole(QPalette::Dark);
    scrollAreaRight->setWidget(imageLabelRight);

    // Do labels
    sourceFolderLabel = new QLabel("Source Folder: ");
    destinationFolderLabel = new QLabel("Destination Folder: ");

    sourceFolderLineEdit = new QLineEdit();
    destinationFolderLineEdit = new QLineEdit();

    // Do the layout
    QWidget *centralWidget = new QWidget;

    startButton = new QPushButton("Start");
    sourceFolderButton = new QPushButton("Open Folder");
    destinationFolderButton = new QPushButton("Open Folder");

    QObject::connect(startButton, &QPushButton::clicked, this, &ImageViewer::sortSetup);
    QObject::connect(sourceFolderButton, &QPushButton::clicked, this, [=](){ this->sourceFolderLineEdit->setText(this->openDirectory()); });
    QObject::connect(destinationFolderButton, &QPushButton::clicked, this, [=](){ this->destinationFolderLineEdit->setText(this->openDirectory()); });

    QGridLayout *infoLayout = new QGridLayout();
    infoLayout->addWidget(sourceFolderLabel, 0, 0);
    infoLayout->addWidget(sourceFolderLineEdit, 0, 1);
    infoLayout->addWidget(sourceFolderButton, 0, 2);
    infoLayout->addWidget(destinationFolderLabel, 1, 0);
    infoLayout->addWidget(destinationFolderLineEdit, 1, 1);
    infoLayout->addWidget(destinationFolderButton, 1, 2);
    infoLayout->addWidget(startButton);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(scrollAreaLeft);
    layout->addWidget(scrollAreaRight);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(infoLayout);
    mainLayout->addLayout(layout);

    setCentralWidget(centralWidget);

    createActions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    scrollAreaRight->setWidgetResizable(true);
    scrollAreaLeft->setWidgetResizable(true);
}

void ImageViewer::sortSetup()
{
    // Connect the slot and signal
    QObject::connect(imageLabelLeft, &clickimagelabel::mousePressed,
                     this, &ImageViewer::clickImage);
    QObject::connect(imageLabelRight, &clickimagelabel::mousePressed,
                     this, &ImageViewer::clickImage);

    // More test code

    QDir directory(sourceFolderLineEdit->text());
    QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);

    // Setup the jsonData

    QJsonArray level;
    QJsonArray nextLevel;

    QList<QString>::iterator i;
    for(i = images.begin(); i != images.end(); ++i)
    {
        QJsonArray tmpArray;
        tmpArray.append(*i);
        level.append(tmpArray);
    }

    QJsonArray empty;
    nextLevel.append(empty);

    sortData["currentLevel"] = level;
    sortData["nextLevel"] = nextLevel;

    //Debug
    QString strFromObj = QJsonDocument(sortData).toJson(QJsonDocument::Compact).toStdString().c_str();
    qDebug() << strFromObj;

    loadFile(sourceFolderLineEdit->text() + "\\" + images[0], false);
    loadFile(sourceFolderLineEdit->text() + "\\" + images[1], true);
}

bool ImageViewer::loadFile(const QString &fileName, const bool right)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage, right);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void ImageViewer::setImage(const QImage &newImage, const bool right)
{
    if(right)
    {
        image = newImage;
        imageLabelRight->setPixmap(QPixmap::fromImage(image));
        scaleFactor = 1.0;


        scrollAreaRight->setVisible(true);
        printAct->setEnabled(true);
        fitToWindowAct->setEnabled(true);
        updateActions();

        if (!fitToWindowAct->isChecked())
            imageLabelRight->adjustSize();
    }
    else
    {
        image = newImage;
        imageLabelLeft->setPixmap(QPixmap::fromImage(image));
        scaleFactor = 1.0;

        scrollAreaLeft->setVisible(true);
        printAct->setEnabled(true);
        fitToWindowAct->setEnabled(true);
        updateActions();

        if (!fitToWindowAct->isChecked())
            imageLabelLeft->adjustSize();
    }
}


bool ImageViewer::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}


QString ImageViewer::openDirectory()
{
    return QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                QDir::homePath(),
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
}

void ImageViewer::open()
{
    QFileDialog dialog(this, tr("Open Directory"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first(), true)) {}
}

void ImageViewer::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::print()
{
    Q_ASSERT(imageLabelRight->pixmap());
#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabelRight->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabelRight->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabelRight->pixmap());
    }
#endif
}

void ImageViewer::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void ImageViewer::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage, true);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabelRight->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollAreaRight->setWidgetResizable(fitToWindow);
    scrollAreaLeft->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::openDirectory);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    saveAsAct->setEnabled(false);

    printAct = fileMenu->addAction(tr("&Print..."), this, &ImageViewer::print);
    printAct->setShortcut(QKeySequence::Print);
    printAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    copyAct = editMenu->addAction(tr("&Copy"), this, &ImageViewer::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &ImageViewer::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
    helpMenu->addAction(tr("About &Qt"), &QApplication::aboutQt);
}

void ImageViewer::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabelRight->pixmap());
    scaleFactor *= factor;
    imageLabelRight->resize(scaleFactor * imageLabelRight->pixmap()->size());

    adjustScrollBar(scrollAreaRight->horizontalScrollBar(), factor);
    adjustScrollBar(scrollAreaRight->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

/**
 * @brief ImageViewer::clickImage
 * @param val
 *
 * This method assumed that the sortData member has the currentLevel json array set with at least
 * two elements. This method also assumes that the sortData memebr has the nextLevel attribute set
 * with at least an empty nested array.
 */
void ImageViewer::clickImage(QString val)
{
    QString strFromObj;
//    strFromObj = QJsonDocument(sortData).toJson(QJsonDocument::Compact).toStdString().c_str();
//    qDebug() << strFromObj;

    QJsonArray level, nextLevel, leaf, otherLeaf, nextLeaf, tmpLeaf, leaf1, leaf2;
    QString value;
    QJsonArray::iterator i;
    int curLeafPos, otherLeafPos;

//    level = sortData["currentLevel"].toArray();

    level = sortData["currentLevel"].toArray();
    nextLevel = sortData["nextLevel"].toArray();
    nextLeaf = nextLevel.last().toArray();

    if(val == "Left")
    {
        leaf = level.first().toArray();
        value = leaf.first().toString();
        leaf.removeFirst();
        level[0] = leaf;
        nextLeaf.append(value);
        otherLeaf = level[1].toArray();
        curLeafPos = 0;
        otherLeafPos = 1;
    }
    else // val == "Right"
    {
        leaf = level[1].toArray();
        value = leaf.first().toString();
        leaf.removeFirst();
        level[1] = leaf;
        nextLeaf.append(value);
        otherLeaf = level.first().toArray();
        curLeafPos = 1;
        otherLeafPos = 0;
    }

    // Append the rest to the next level leaf if nothing left to compare
    if(leaf.size() == 0)
    {
        for(i = otherLeaf.begin(); i != otherLeaf.end(); ++i)
        {
            nextLeaf.append((*i).toString());
        }

        level.removeFirst();
        level.removeFirst();

        nextLevel.removeLast();
        nextLevel.append(nextLeaf);
        QJsonArray tmpArray;
        nextLevel.append(tmpArray);
    }
    else
    {
        nextLevel.removeLast();
        nextLevel.append(nextLeaf);
    }

    // Stuff to do after performing the sorting step.
    if(level.size() > 1)
    {
        sortData["currentLevel"] = level;
        sortData["nextLevel"] = nextLevel;
    }
    else if(level.size() == 1)
    {
        tmpLeaf = nextLevel.at(nextLevel.size() - 2).toArray();
        nextLevel.removeAt(nextLevel.size() - 2);

        level.prepend(tmpLeaf);

        sortData["currentLevel"] = level;
        sortData["nextLevel"] = nextLevel;
    }
    else // level.size() == 0
    {
        nextLevel.removeLast();
        sortData["currentLevel"] = nextLevel;
        QJsonArray empty;
        nextLevel = empty;
        nextLevel.append(empty);
        sortData["nextLevel"] = nextLevel;
    }

    level = sortData["currentLevel"].toArray();
    nextLevel = sortData["nextLevel"].toArray();

    if(level.size() == 1 && nextLevel.size() == 1 && nextLevel[0].toArray().size() == 0)
    {
        copySortedFiles();
        qDebug() << "Hello :-)";
    }
    else
    {
        level = sortData["currentLevel"].toArray();
        leaf1 = level.first().toArray();
        level.removeFirst();
        leaf2 = level.first().toArray();
        level.removeFirst();

        //Debug
        //    qDebug() << leaf1[0];
        //    qDebug() << leaf2[0];
//        strFromObj = QJsonDocument(sortData).toJson(QJsonDocument::Compact).toStdString().c_str();
//        qDebug() << strFromObj;

        loadFile(sourceFolderLineEdit->text() + "\\" + leaf1[0].toString(), false);
        loadFile(sourceFolderLineEdit->text() + "\\" + leaf2[0].toString(), true);
    }

//    loadFile("C:\\Users\\Switch\\Pictures\\" + images[2], false);
//    loadFile("C:\\Users\\Switch\\Pictures\\" + images[3], true);
}

void ImageViewer::copySortedFiles()
{
    QJsonArray level, list;
    level = sortData["currentLevel"].toArray();
    list = level[0].toArray();
    int count = 1;

    QString strFromObj = QJsonDocument(list).toJson(QJsonDocument::Compact).toStdString().c_str();
    qDebug() << strFromObj;

    QJsonArray::iterator i;
    for(i = list.begin(); i != list.end(); ++i)
    {
        QFile::copy(sourceFolderLineEdit->text() + "\\" + (*i).toString(), destinationFolderLineEdit->text() + "\\" + QString::number(i - list.begin() + 1) + ".jpg");
    }
}















