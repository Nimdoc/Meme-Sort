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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "clickimagelabel.h"
#include "analysishelper.h"

#include <QMainWindow>
#include <QImage>
#include <QJsonObject>
#include <QLineEdit>
#include <QPushButton>
#include <QUndoStack>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = nullptr);
    bool loadFile(const QString &, const bool right);

    void setSortState(QJsonObject data, int newComparisons);

private slots:
    QString openDirectory();
    void about();

    // For decided which image is better
    void clickImage(QString val);
    void startSort();
    void stopSort();
    void sortSetup();
    void copySortedFiles();
    void loadImagesFromCurrentLevel();

private:
    void setImage(const QImage &newImage, const bool right);

    QImage image;
    clickimagelabel *imageLabelRight;
    clickimagelabel *imageLabelLeft;
    QScrollArea *scrollAreaRight;
    QScrollArea *scrollAreaLeft;
    double scaleFactor;

    // Start button
    QPushButton *aboutButton;
    QPushButton *sourceFolderButton;
    QPushButton *destinationFolderButton;
    QPushButton *startButton;
    QPushButton *undoButton;
    QPushButton *redoButton;

    QUndoStack *undoStack = nullptr;

    // Data for images and sorting
    QJsonObject sortData;

    //Source and Destination members
    QLabel *sourceFolderLabel;
    QLabel *destinationFolderLabel;

    QLineEdit *sourceFolderLineEdit;
    QLineEdit *destinationFolderLineEdit;

    int comparisons = 0;
    int initialSize = 0;
    int lowerBound;
    int upperBound;
    bool isSorting = false;

    QAction *undoAction = nullptr;
    QAction *redoAction = nullptr;
};

#endif
