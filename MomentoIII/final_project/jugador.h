#ifndef JUGADOR_H
#define JUGADOR_H

#include <QPointF>

#include "personaje.h"
#include "proyectil.h"

/*
    Clase Jugador

    Hereda de Personaje y agrupa las acciones propias del personaje
    controlado por el usuario: batear, controlar la bola y activar habilidad.
    No cambia las físicas existentes; solo mueve responsabilidades que antes
    quedaban cargadas en MainWindow.
*/
class Jugador : public Personaje
{
public:
    Jugador(const QString &rutaQuieto,int framesQuieto,const QString &rutaAtaque,int framesAtaque,qreal posX,qreal posY,float vidaMaxima);

    bool bolaEnZonaBateo(Proyectil *bola) const;
    bool batear(Proyectil *bola, const QPointF &destino);
    void controlarProyectil(Proyectil *bola, int tecla);
    void activarHabilidad();

    void setPuedeBatear(bool puede);
    bool getPuedeBatear() const;

    void setBolaControlada(Proyectil *bola);
    Proyectil *getBolaControlada() const;

private:
    Proyectil *bolaControlada;
    float fuerzaBateo;
    bool puedeBatear;
};

#endif // JUGADOR_H
