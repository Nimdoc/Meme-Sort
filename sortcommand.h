#ifndef SORTCOMMAND_H
#define SORTCOMMAND_H

#include "imageviewer.h"

#include <QJsonObject>
#include <QUndoCommand>

class SortCommand : public QUndoCommand
{
public:
    explicit SortCommand(ImageViewer *imgViewer, QJsonObject data, int newComparisons);

    void undo() override;
    void redo() override;

private:
    QJsonObject sortState;
    int comparisons;

    ImageViewer *imageViewer;
};

#endif // SORTCOMMAND_H
