#include "sprite.h"
Sprite::Sprite(const QString &rutaImagen, int cantidadFrames, QObject *parent) : QObject{parent}
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

    connect(timer, &QTimer::timeout, this, &Sprite::actualizacion);
}

void Sprite::iniciarAnimacion()
{
    frameActual = 0;
    update();
    timer->start(100);
}

void Sprite::actualizacion()
{
    frameActual++;

    if(frameActual >= totalFrames){
        frameActual = 0;
        timer->stop();
    }

    this->update(-ancho/2, -alto/2, ancho, alto);
}

QRectF Sprite::boundingRect() const
{
    return QRectF(-ancho/2, -alto/2, ancho, alto);
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const int origenX = frameActual * static_cast<int>(ancho);
    painter->drawPixmap(QPointF(-ancho / 2, -alto / 2), *pixmap, QRectF(origenX, 0, ancho, alto));
}
