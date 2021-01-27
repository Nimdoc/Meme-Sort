#include "clickimagelabel.h"

#include <QMainWindow>
#include <QMouseEvent>
#include <QWidget>
#include <QtDebug>

clickimagelabel::clickimagelabel( QWidget * parent, Qt::WindowFlags f ) : QLabel( parent, f ) {}
clickimagelabel::clickimagelabel( const QString& text, QWidget* parent, Qt::WindowFlags f ) : QLabel( text, parent, f ) {}

void clickimagelabel::mousePressEvent( QMouseEvent* ev )
{
    const QPoint p = ev->pos();
    emit mousePressed( name );

//    QString x = QString::number(ev->x());
//    QString y = QString::number(ev->y());
//    qDebug() << x << "," << y;
//    qDebug() << name;
}

void clickimagelabel::setName(QString newName)
{
    name = newName;
}

QString clickimagelabel::getName()
{
    return name;
}
