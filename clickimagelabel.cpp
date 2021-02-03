#include "clickimagelabel.h"

#include <QMainWindow>
#include <QMouseEvent>
#include <QWidget>
#include <QtDebug>

clickimagelabel::clickimagelabel( QWidget * parent, Qt::WindowFlags f ) : QLabel( parent, f ) {}
clickimagelabel::clickimagelabel( const QString& text, QWidget* parent, Qt::WindowFlags f ) : QLabel( text, parent, f ) {}

void clickimagelabel::mousePressEvent( QMouseEvent* ev )
{
    emit mousePressed( name );
}

void clickimagelabel::setName(QString newName)
{
    name = newName;
}

QString clickimagelabel::getName()
{
    return name;
}
