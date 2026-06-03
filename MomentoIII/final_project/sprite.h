#ifndef SPRITE_H
#define SPRITE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPixmap>
#include <QString>

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

    /*
        Regresa la animación al primer frame.
    */
    void reiniciarAnimacion();

    /*
        Avanza un frame.

        Retorna true cuando ya llegó al final de la animación.
    */
    bool avanzarFrame();

    int getTotalFrames() const;

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
};

#endif // SPRITE_H
