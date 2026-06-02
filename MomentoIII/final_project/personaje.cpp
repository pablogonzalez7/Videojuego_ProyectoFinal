#include "personaje.h"

Personaje::Personaje(const QString &rutaImagen, int cantidadFrames, qreal posX, qreal posY)
{
    // Cada personaje encapsula un Sprite que contiene la imagen y la animación.
    spritePersonaje = new Sprite(rutaImagen, cantidadFrames);

    // Se posiciona desde el inicio para que al agregarlo a la escena ya aparezca
    // en el lugar correcto del mapa.
    spritePersonaje->setPos(posX, posY);
}

void Personaje::agregarAEscena(QGraphicsScene *scene)
{
    // La validación evita referencias nulas y agregar dos veces el mismo item.
    if(scene != nullptr && spritePersonaje->scene() != scene){
        scene->addItem(spritePersonaje);
    }
}

Sprite *Personaje::getSprite() const
{
    // MainWindow usa este puntero para iniciar animaciones y revisar colisiones.
    return spritePersonaje;
}
