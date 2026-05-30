#include "personaje.h"

Personaje::Personaje(const QString &rutaImagen, int cantidadFrames, qreal posX, qreal posY)
{
    spritePersonaje = new Sprite(rutaImagen, cantidadFrames);
    spritePersonaje->setPos(posX, posY);
}

void Personaje::agregarAEscena(QGraphicsScene *scene)
{
    if(scene != nullptr && spritePersonaje->scene() != scene){
        scene->addItem(spritePersonaje);
    }
}

Sprite *Personaje::getSprite() const
{
    return spritePersonaje;
}
