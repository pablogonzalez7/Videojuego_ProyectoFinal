#ifndef SPRITE_H
#define SPRITE_H

#include <QObject>
#include <QString>

#include "QGraphicsItem"
#include "QTimer"
#include "QPixmap"
#include "QPainter"

class sprite : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit sprite(const QString &rutaImagen, int cantidadFrames, QObject *parent = nullptr);

    QTimer *timer;
    QPixmap *pixmap;

    int frameActual;
    int totalFrames;

    qreal ancho;
    qreal alto;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    void actualizacion();
};

#endif // SPRITE_H
