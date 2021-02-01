#include "sortcommand.h"

SortCommand::SortCommand(ImageViewer *imgViewer, QJsonObject data, int newComparisons)
{
    imageViewer = imgViewer;
    sortState = data;
    comparisons = newComparisons;
}

void SortCommand::undo()
{
    imageViewer->setSortState(sortState, comparisons);
}

void SortCommand::redo()
{
    imageViewer->setSortState(sortState, comparisons);
}


