#include "sprite.h"

Sprite::Sprite(const QString &rutaImagen, int cantidadFrames)
{
    cargarHoja(rutaImagen, cantidadFrames);
}

void Sprite::cambiarSprite(const QString &rutaImagen, int cantidadFrames)
{
    prepareGeometryChange();
    cargarHoja(rutaImagen, cantidadFrames);
    update();
}

void Sprite::cargarHoja(const QString &rutaImagen, int cantidadFrames)
{
    frameActual = 0;
    totalFrames = cantidadFrames;

    if (totalFrames <= 0) {
        totalFrames = 1;
    }

    pixmap = QPixmap(rutaImagen);

    ancho = pixmap.width() / totalFrames;
    alto = pixmap.height();
}

void Sprite::reiniciarAnimacion()
{
    frameActual = 0;
    update();
}

bool Sprite::avanzarFrame()
{
    if (frameActual >= totalFrames - 1) {
        return true;
    }

    frameActual++;
    update();
    return false;
}

int Sprite::getTotalFrames() const
{
    return totalFrames;
}

int Sprite::getFrameActual() const
{
    return frameActual;
}

QRectF Sprite::boundingRect() const
{
    return QRectF(-ancho / 2, -alto / 2, ancho, alto);
}

void Sprite::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const int origenX = frameActual * static_cast<int>(ancho);

    painter->drawPixmap(QPointF(-ancho / 2, -alto / 2),pixmap,QRectF(origenX, 0, ancho, alto));
}
