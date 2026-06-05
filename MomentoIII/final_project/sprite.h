#ifndef SPRITE_H
#define SPRITE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QVector>

/*
    Clase Sprite

    Representa una hoja de sprites horizontal dentro de la escena.
    Esta versión es un item gráfico puro: no tiene timers ni slots propios.
*/
class Sprite : public QGraphicsItem
{
public:
    explicit Sprite(const QString &rutaImagen, int cantidadFrames);

    /*
        Cambia la hoja de sprites completa.

        Se usa cuando Vegito entra en kaioken o cuando un personaje
        necesita mostrar otra animación distinta.
    */
    void cambiarSprite(const QString &rutaImagen, int cantidadFrames);
    void cambiarSprite(const QString &rutaImagen, const QVector<QRectF> &framesPersonalizados);

    /*
        Regresa la animación al primer frame.
    */
    void reiniciarAnimacion();

    /*
        Avanza un frame.

        Retorna true cuando ya llegó al final de la animación.
    */
    bool avanzarFrame();
    void setMantenerTamanoVisual(bool mantener);
    void fijarTamanoVisual(qreal anchoVisible, qreal altoVisible);
    void limpiarTamanoVisualFijo();

    int getTotalFrames() const;
    int getFrameActual() const;

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    void cargarHoja(const QString &rutaImagen, int cantidadFrames);

    QPixmap pixmap;
    int frameActual;
    int totalFrames;
    qreal ancho;
    qreal alto;
    qreal anchoBase;
    qreal altoBase;
    bool mantenerTamanoVisual;
    bool usarTamanoVisualFijo;
    qreal anchoVisualFijo;
    qreal altoVisualFijo;
    QVector<QRectF> framesPersonalizados;
};

#endif // SPRITE_H
