#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsScene>
#include <QString>

#include "sprite.h"

/*
    Clase Personaje

    Sirve como una capa simple sobre Sprite para representar entidades del juego
    como Vegito y Freezer. De esta forma MainWindow trabaja con personajes a
    nivel lógico y delega en Sprite el dibujo y la animación cuadro a cuadro.
*/
class Personaje
{
public:
    // Crea el personaje, carga su hoja de sprites y fija su posición inicial.
    Personaje(const QString &rutaImagen, int cantidadFrames, qreal posX, qreal posY);

    // Inserta el sprite en la escena solo si la escena es válida y el personaje
    // todavía no pertenece a esa misma escena.
    void agregarAEscena(QGraphicsScene *scene);

    // Expone el sprite interno para animaciones, posición y colisiones.
    Sprite *getSprite() const;

private:
    // Item gráfico que representa al personaje dentro de la escena.
    Sprite *spritePersonaje;
};

#endif // PERSONAJE_H
