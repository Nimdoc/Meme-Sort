#ifndef SORTCOMMAND_H
#define SORTCOMMAND_H

#include "memesort.h"

#include <QJsonObject>
#include <QUndoCommand>

class SortCommand : public QUndoCommand
{
public:
    explicit SortCommand(MemeSort *imgViewer, QJsonObject data, int newComparisons);

    void undo() override;
    void redo() override;

private:
    QJsonObject sortState;
    int comparisons;

    MemeSort *imageViewer;
};

#endif // SORTCOMMAND_H
