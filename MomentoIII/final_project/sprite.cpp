#include "sprite.h"

Sprite::Sprite(const QString &rutaImagen, int cantidadFrames)
{
    anchoBase = 0;
    altoBase = 0;
    mantenerTamanoVisual = false;
    usarTamanoVisualFijo = false;
    anchoVisualFijo = 0;
    altoVisualFijo = 0;
    cargarHoja(rutaImagen, cantidadFrames);
}

void Sprite::cambiarSprite(const QString &rutaImagen, int cantidadFrames)
{
    prepareGeometryChange();
    framesPersonalizados.clear();
    cargarHoja(rutaImagen, cantidadFrames);
    update();
}

void Sprite::cambiarSprite(const QString &rutaImagen, const QVector<QRectF> &nuevosFramesPersonalizados)
{
    prepareGeometryChange();
    framesPersonalizados = nuevosFramesPersonalizados;
    cargarHoja(rutaImagen, framesPersonalizados.size());
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

    if (mantenerTamanoVisual && (anchoBase <= 0 || altoBase <= 0)) {
        anchoBase = ancho;
        altoBase = alto;
    }
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

void Sprite::setMantenerTamanoVisual(bool mantener)
{
    prepareGeometryChange();
    mantenerTamanoVisual = mantener;

    if (mantenerTamanoVisual) {
        anchoBase = ancho;
        altoBase = alto;
    }

    update();
}

void Sprite::fijarTamanoVisual(qreal anchoVisible, qreal altoVisible)
{
    prepareGeometryChange();
    usarTamanoVisualFijo = true;
    anchoVisualFijo = anchoVisible;
    altoVisualFijo = altoVisible;
    update();
}

void Sprite::limpiarTamanoVisualFijo()
{
    prepareGeometryChange();
    usarTamanoVisualFijo = false;
    anchoVisualFijo = 0;
    altoVisualFijo = 0;
    update();
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
    qreal anchoFrameActual = ancho;
    qreal altoFrameActual = alto;

    if (!framesPersonalizados.isEmpty() &&
        frameActual >= 0 &&
        frameActual < framesPersonalizados.size()) {
        anchoFrameActual = framesPersonalizados.at(frameActual).width();
        altoFrameActual = framesPersonalizados.at(frameActual).height();
    }

    const qreal anchoVisible = usarTamanoVisualFijo ? anchoVisualFijo : (mantenerTamanoVisual ? anchoBase : anchoFrameActual);
    const qreal altoVisible = usarTamanoVisualFijo ? altoVisualFijo : (mantenerTamanoVisual ? altoBase : altoFrameActual);
    return QRectF(-anchoVisible / 2, -altoVisible / 2, anchoVisible, altoVisible);
}

void Sprite::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF frameOrigen(frameActual * ancho, 0, ancho, alto);

    if (!framesPersonalizados.isEmpty() &&
        frameActual >= 0 &&
        frameActual < framesPersonalizados.size()) {
        frameOrigen = framesPersonalizados.at(frameActual);
    }

    const qreal anchoVisible = usarTamanoVisualFijo ? anchoVisualFijo : (mantenerTamanoVisual ? anchoBase : frameOrigen.width());
    const qreal altoVisible = usarTamanoVisualFijo ? altoVisualFijo : (mantenerTamanoVisual ? altoBase : frameOrigen.height());

    QRectF destino(-anchoVisible / 2, -altoVisible / 2, anchoVisible, altoVisible);

    if (usarTamanoVisualFijo && frameOrigen.width() > 0.0 && frameOrigen.height() > 0.0) {
        const qreal escalaX = anchoVisible / frameOrigen.width();
        const qreal escalaY = altoVisible / frameOrigen.height();
        const qreal escala = qMin(escalaX, escalaY);
        const qreal anchoAjustado = frameOrigen.width() * escala;
        const qreal altoAjustado = frameOrigen.height() * escala;
        destino = QRectF(-anchoAjustado / 2, -altoAjustado / 2, anchoAjustado, altoAjustado);
    }

    painter->drawPixmap(destino, pixmap, frameOrigen);
}
