#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsScene>
#include <QString>

#include "sprite.h"

class Personaje
{
public:
    Personaje(const QString &rutaImagen, int cantidadFrames, qreal posX, qreal posY);

    void agregarAEscena(QGraphicsScene *scene);
    Sprite *getSprite() const;

private:
    Sprite *spritePersonaje;
};

#endif // PERSONAJE_H
