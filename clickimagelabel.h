#ifndef CLICKIMAGELABEL_H
#define CLICKIMAGELABEL_H

#include <QLabel>
#include <QObject>
#include <QString>


class clickimagelabel : public QLabel
{
Q_OBJECT
signals:
    void mousePressed( const QString );

public:
    clickimagelabel( QWidget* parent = nullptr, Qt::WindowFlags f = nullptr );
    clickimagelabel( const QString& text, QWidget* parent = nullptr, Qt::WindowFlags f = nullptr );

    void mousePressEvent( QMouseEvent* ev );

    void setName(QString newName);
    QString getName();

private:
    QString name = "";
};

#endif // CLICKIMAGELABEL_H
