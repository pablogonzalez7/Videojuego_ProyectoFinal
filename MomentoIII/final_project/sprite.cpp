#include "sprite.h"
sprite::sprite(const QString &rutaImagen, int cantidadFrames, QObject *parent) : QObject{parent}
{
    timer = new QTimer(this);

    frameActual = 0;
    totalFrames = cantidadFrames;

    pixmap = new QPixmap(rutaImagen);

    if(totalFrames <= 0){
        totalFrames = 1;
    }

    ancho = pixmap->width() / totalFrames;
    alto = pixmap->height();

    timer->start(250);

    connect(timer, &QTimer::timeout, this, &sprite::actualizacion);
}

void sprite::actualizacion()
{
    frameActual = (frameActual + 1) % totalFrames;
    this->update(-ancho/2, -alto/2, ancho, alto);
}

QRectF sprite::boundingRect() const
{
    return QRectF(-ancho/2, -alto/2, ancho, alto);
}

void sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const int origenX = frameActual * static_cast<int>(ancho);
    painter->drawPixmap(QPointF(-ancho / 2, -alto / 2), *pixmap, QRectF(origenX, 0, ancho, alto));
}
