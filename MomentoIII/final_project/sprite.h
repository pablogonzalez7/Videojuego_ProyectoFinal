#ifndef SPRITE_H
#define SPRITE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QVector>

class Sprite : public QGraphicsItem
{
public:
    explicit Sprite(const QString &rutaImagen, int cantidadFrames);


    void cambiarSprite(const QString &rutaImagen, int cantidadFrames);
    void cambiarSprite(const QString &rutaImagen, const QVector<QRectF> &framesPersonalizados);


    void reiniciarAnimacion();


    bool avanzarFrame();
    void setMantenerTamañoVisual(bool mantener);
    void fijarTamañoVisual(qreal anchoVisible, qreal altoVisible);
    void limpiarTamañoVisualFijo();

    int getTotalFrames() const;
    int getFrameActual() const;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void cargarHoja(const QString &rutaImagen, int cantidadFrames);

    QPixmap pixmap;
    int frameActual;
    int totalFrames;
    qreal ancho;
    qreal alto;
    qreal anchoBase;
    qreal altoBase;
    bool mantenerTamañoVisual;
    bool usarTamañoVisualFijo;
    qreal anchoVisualFijo;
    qreal altoVisualFijo;
    QVector<QRectF> framesPersonalizados;
};

#endif
