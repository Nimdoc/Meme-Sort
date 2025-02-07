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
#include "sortcommand.h"

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
    imageLabelLeft->setAlignment(Qt::AlignCenter);

    scrollAreaLeft = new QScrollArea;
    scrollAreaLeft->setBackgroundRole(QPalette::Dark);
    scrollAreaLeft->setWidget(imageLabelLeft);

    // Right image
    imageLabelRight = new clickimagelabel;
    imageLabelRight->setName("Right");
    imageLabelRight->setBackgroundRole(QPalette::Base);
    imageLabelRight->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabelRight->setAlignment(Qt::AlignCenter);

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

    undoStack = new QUndoStack(this);

    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    aboutButton = new QPushButton("About");
    aboutButton->setFixedWidth(60);
    startButton = new QPushButton("Start");
    undoButton = new QPushButton(tr("⟲"));
    undoButton->setFixedWidth(40);
    redoButton = new QPushButton(tr("⟳"));
    redoButton->setFixedWidth(40);
    sourceFolderButton = new QPushButton("Open Folder");
    destinationFolderButton = new QPushButton("Open Folder");

    QObject::connect(aboutButton, &QPushButton::clicked, this, [=](){ this->about(); });
    QObject::connect(startButton, &QPushButton::clicked, this, [=](){ this->isSorting ? this->stopSort() : this->startSort(); });
    QObject::connect(undoButton, &QPushButton::clicked, this, [=](){ undoStack->undo(); });
    QObject::connect(redoButton, &QPushButton::clicked, this, [=](){ undoStack->redo(); });
    QObject::connect(sourceFolderButton, &QPushButton::clicked, this, [=](){ this->sourceFolderLineEdit->setText(this->openDirectory()); });
    QObject::connect(destinationFolderButton, &QPushButton::clicked, this, [=](){ this->destinationFolderLineEdit->setText(this->openDirectory()); });

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(undoButton);
    controlsLayout->addWidget(redoButton);

    QGridLayout *infoLayout = new QGridLayout();
    infoLayout->addWidget(sourceFolderLabel, 0, 0);
    infoLayout->addWidget(sourceFolderLineEdit, 0, 1);
    infoLayout->addWidget(sourceFolderButton, 0, 2);
    infoLayout->addWidget(destinationFolderLabel, 1, 0);
    infoLayout->addWidget(destinationFolderLineEdit, 1, 1);
    infoLayout->addWidget(destinationFolderButton, 1, 2);
    infoLayout->addLayout(controlsLayout, 2, 0);
    infoLayout->addWidget(aboutButton, 2, 1);
    infoLayout->addWidget(startButton, 2, 2);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(scrollAreaLeft);
    layout->addWidget(scrollAreaRight);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(infoLayout);
    mainLayout->addLayout(layout);

    setCentralWidget(centralWidget);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    scrollAreaRight->setWidgetResizable(true);
    scrollAreaLeft->setWidgetResizable(true);
}

void ImageViewer::startSort()
{
    bool srcGood = false, destGood = false;

    if(sourceFolderLineEdit->text().length() == 0 || !QDir(sourceFolderLineEdit->text()).exists())
    {
        sourceFolderLineEdit->setStyleSheet("border: 1px solid red");
    }
    else
    {
        sourceFolderLineEdit->setStyleSheet("");
        srcGood = true;
    }

    if(destinationFolderLineEdit->text().length() == 0 || !QDir(destinationFolderLineEdit->text()).exists())
    {
        destinationFolderLineEdit->setStyleSheet("border: 1px solid red");
    }
    else
    {
        destinationFolderLineEdit->setStyleSheet("");
        destGood = true;
    }

    if(srcGood && destGood)
    {
        isSorting = true;
        sourceFolderLineEdit->setEnabled(false);
        destinationFolderLineEdit->setEnabled(false);
        sourceFolderButton->setEnabled(false);
        destinationFolderButton->setEnabled(false);
        startButton->setText("Stop");
        comparisons = 0;
        sortSetup();
        const QString message = tr("Comparisons: %1 | Estimated %2 to %3 comparisons needed.").arg(comparisons).arg(lowerBound).arg(upperBound);
        statusBar()->showMessage(message);
    }
}

void ImageViewer::stopSort()
{
    isSorting = false;

    QObject::disconnect(imageLabelLeft, &clickimagelabel::mousePressed,
                     this, &ImageViewer::clickImage);
    QObject::disconnect(imageLabelRight, &clickimagelabel::mousePressed,
                     this, &ImageViewer::clickImage);

    sourceFolderLineEdit->setEnabled(true);
    destinationFolderLineEdit->setEnabled(true);
    sourceFolderButton->setEnabled(true);
    destinationFolderButton->setEnabled(true);
    startButton->setText("Start");
}

void ImageViewer::sortSetup()
{
    // Connect the slot and signal
    QObject::connect(imageLabelLeft, &clickimagelabel::mousePressed,
                     this, &ImageViewer::clickImage);
    QObject::connect(imageLabelRight, &clickimagelabel::mousePressed,
                     this, &ImageViewer::clickImage);

    QDir directory(sourceFolderLineEdit->text());
    QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.JPG" << "*.png" << "*.PNG",QDir::Files);

    if(images.size() == 0)
    {
        stopSort();
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("There is nothing to sort in %1").arg(sourceFolderLineEdit->text()));
        return;
    }
    else if(images.size() == 1)
    {
        QFileInfo file(sourceFolderLineEdit->text() + "\\" + (images[0]));
        QFile::copy(sourceFolderLineEdit->text() + "\\" + (images[0]), destinationFolderLineEdit->text() + "\\1." + file.completeSuffix());
        stopSort();
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("There was one file found in the source directory. By definition a list of size one is in order. The one file has been copied to the destination folder %1").arg(sourceFolderLineEdit->text()));
        return;
    }

    initialSize = images.size();
    lowerBound = AnalysisHelper::getLowerBound(initialSize);
    upperBound = AnalysisHelper::getUpperBound(initialSize);

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

    return true;
}

void ImageViewer::setImage(const QImage &newImage, const bool right)
{
    if(right)
    {
        image = newImage;
        imageLabelRight->setPixmap(QPixmap::fromImage(image).scaled(imageLabelRight->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        scaleFactor = 1.0;
        scrollAreaRight->setVisible(true);
    }
    else
    {
        image = newImage;
        imageLabelLeft->setPixmap(QPixmap::fromImage(image).scaled(imageLabelLeft->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        scaleFactor = 1.0;
        scrollAreaLeft->setVisible(true);
    }
}

QString ImageViewer::openDirectory()
{
    return QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                QDir::homePath(),
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About"),
                       tr("<p><b>Meme Sort</b> is a sorting program that lets you sort your "
                        "pictures. All you do is select a source and destination folder, "
                        "click start, then click on which image you think is better."
                        " When all the comparisons are made, the pictures from the "
                        "source folder will be copied to the destination folder and "
                        "will be ordered by your judgement.</p>"
                        "<p>Version: 1.0</p>"
                        "Tom Busby 2021 <a href=\"https://busbania.com\">busbania.com<a>"));
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
    QJsonArray level, nextLevel, leaf, otherLeaf, nextLeaf, tmpLeaf, leaf1, leaf2;
    QString value;
    QJsonArray::iterator i;
    int curLeafPos, otherLeafPos;

    // Push the current state on to the undo stack so the following comparison can be undone.
    undoStack->push(new SortCommand(this, sortData, comparisons));

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

    comparisons++;
    const QString message = tr("Comparisons: %1 | Estimated %2 to %3 comparisons needed.").arg(comparisons).arg(lowerBound).arg(upperBound);
    statusBar()->showMessage(message);

    if(level.size() == 1 && nextLevel.size() == 1 && nextLevel[0].toArray().size() == 0)
    {
        copySortedFiles();
        stopSort();
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("All files have been sorted, and have been copied to the destination folder."));
    }
    else
    {
        loadImagesFromCurrentLevel();
    }
}

void ImageViewer::copySortedFiles()
{
    QJsonArray level, list;
    level = sortData["currentLevel"].toArray();
    list = level[0].toArray();

    QJsonArray::iterator i;
    for(i = list.begin(); i != list.end(); ++i)
    {
        QFileInfo file(sourceFolderLineEdit->text() + "\\" + (*i).toString());
        QFile::copy(sourceFolderLineEdit->text() + "\\" + (*i).toString(), destinationFolderLineEdit->text() + "\\" + QString::number(i - list.begin() + 1) + "." + file.completeSuffix());
    }
}

void ImageViewer::loadImagesFromCurrentLevel()
{
        QJsonArray level, leaf1, leaf2;
        level = sortData["currentLevel"].toArray();
        leaf1 = level.first().toArray();
        level.removeFirst();
        leaf2 = level.first().toArray();
        level.removeFirst();

        loadFile(sourceFolderLineEdit->text() + "\\" + leaf1[0].toString(), false);
        loadFile(sourceFolderLineEdit->text() + "\\" + leaf2[0].toString(), true);
}

void ImageViewer::setSortState(QJsonObject data, int newComparisons)
{
    sortData = data;
    comparisons = newComparisons;
    loadImagesFromCurrentLevel();
    const QString message = tr("Comparisons: %1 | Estimated %2 to %3 comparisons needed.").arg(comparisons).arg(lowerBound).arg(upperBound);
    statusBar()->showMessage(message);
}













