#include "sprite.h"

#include <QHash>
#include <stdexcept>

namespace {
QPixmap obtenerPixmapSprite(const QString &rutaImagen)
{
    static QHash<QString, QPixmap> cacheSprites;

    if (cacheSprites.contains(rutaImagen)) {
        return cacheSprites.value(rutaImagen);
    }

    QPixmap nuevoPixmap(rutaImagen);

    if (nuevoPixmap.isNull()) {
        throw std::runtime_error(("No se pudo cargar la hoja de sprites: " +
                                  rutaImagen).toStdString());
    }

    cacheSprites.insert(rutaImagen, nuevoPixmap);
    return nuevoPixmap;
}
}

Sprite::Sprite(const QString &rutaImagen, int cantidadFrames)
{
    anchoBase = 0;
    altoBase = 0;
    mantenerTamañoVisual = false;
    usarTamañoVisualFijo = false;
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
        throw std::runtime_error(("La hoja de sprites debe tener al menos un frame: " +
                                  rutaImagen).toStdString());
    }

    pixmap = obtenerPixmapSprite(rutaImagen);

    if (pixmap.width() < totalFrames) {
        throw std::runtime_error(("La hoja de sprites tiene menos pixeles de ancho que frames: " +
                                  rutaImagen).toStdString());
    }

    ancho = pixmap.width() / totalFrames;
    alto = pixmap.height();

    if (ancho <= 0 || alto <= 0) {
        throw std::runtime_error(("La hoja de sprites tiene dimensiones invalidas: " +
                                  rutaImagen).toStdString());
    }

    if (mantenerTamañoVisual && (anchoBase <= 0 || altoBase <= 0)) {
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

void Sprite::setMantenerTamañoVisual(bool mantener)
{
    prepareGeometryChange();
    mantenerTamañoVisual = mantener;

    if (mantenerTamañoVisual) {
        anchoBase = ancho;
        altoBase = alto;
    }

    update();
}

void Sprite::fijarTamañoVisual(qreal anchoVisible, qreal altoVisible)
{
    prepareGeometryChange();
    usarTamañoVisualFijo = true;
    anchoVisualFijo = anchoVisible;
    altoVisualFijo = altoVisible;
    update();
}

void Sprite::limpiarTamañoVisualFijo()
{
    prepareGeometryChange();
    usarTamañoVisualFijo = false;
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

    const qreal anchoVisible = usarTamañoVisualFijo ? anchoVisualFijo : (mantenerTamañoVisual ? anchoBase : anchoFrameActual);
    const qreal altoVisible = usarTamañoVisualFijo ? altoVisualFijo : (mantenerTamañoVisual ? altoBase : altoFrameActual);
    return QRectF(-anchoVisible / 2, -altoVisible / 2, anchoVisible, altoVisible);
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF frameOrigen(frameActual * ancho, 0, ancho, alto);

    if (!framesPersonalizados.isEmpty() &&
        frameActual >= 0 &&
        frameActual < framesPersonalizados.size()) {
        frameOrigen = framesPersonalizados.at(frameActual);
    }

    const qreal anchoVisible = usarTamañoVisualFijo ? anchoVisualFijo : (mantenerTamañoVisual ? anchoBase : frameOrigen.width());
    const qreal altoVisible = usarTamañoVisualFijo ? altoVisualFijo : (mantenerTamañoVisual ? altoBase : frameOrigen.height());

    QRectF destino(-anchoVisible / 2, -altoVisible / 2, anchoVisible, altoVisible);

    if (usarTamañoVisualFijo && frameOrigen.width() > 0.0 && frameOrigen.height() > 0.0) {
        const qreal escalaX = anchoVisible / frameOrigen.width();
        const qreal escalaY = altoVisible / frameOrigen.height();
        const qreal escala = qMin(escalaX, escalaY);
        const qreal anchoAjustado = frameOrigen.width() * escala;
        const qreal altoAjustado = frameOrigen.height() * escala;
        destino = QRectF(-anchoAjustado / 2, -altoAjustado / 2, anchoAjustado, altoAjustado);
    }

    painter->drawPixmap(destino, pixmap, frameOrigen);
}
