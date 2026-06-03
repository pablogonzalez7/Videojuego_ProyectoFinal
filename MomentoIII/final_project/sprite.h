#ifndef SPRITE_H
#define SPRITE_H

#include <QObject>
#include <QString>

#include "QGraphicsItem"
#include "QTimer"
#include "QPixmap"
#include "QPainter"

/*
    Clase Sprite

    Implementa un objeto animado dentro de QGraphicsScene a partir de una hoja
    de sprites horizontal. Hereda de QGraphicsItem para poder dibujarse y de
    QObject para integrarse con timers y el sistema de slots de Qt.
*/
class Sprite : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    // Carga la imagen con todos los frames y calcula el tamaño de cada cuadro.
    explicit Sprite(const QString &rutaImagen, int cantidadFrames, QObject *parent = nullptr);

    // Reinicia la animación y arranca el timer que avanza los frames.
    void iniciarAnimacion();

    // Controla cuándo pasar al siguiente frame.
    QTimer *timer;

    // Hoja completa de sprites de la animación.
    QPixmap *pixmap;

    // Frame actualmente visible.
    int frameActual;

    // Número total de frames contenidos en la hoja.
    int totalFrames;

    // Medidas de un frame individual.
    qreal ancho;
    qreal alto;

    // Define el área ocupada por el item para colisiones y repintado.
    QRectF boundingRect() const;

    // Dibuja en pantalla únicamente el frame seleccionado.
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    // Avanza la animación un frame y solicita redibujado.
    void actualizacion();
};

#endif // SPRITE_H
