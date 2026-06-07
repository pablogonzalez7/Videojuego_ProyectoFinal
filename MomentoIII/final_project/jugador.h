#ifndef JUGADOR_H
#define JUGADOR_H

#include <QPointF>

#include "personaje.h"
#include "proyectil.h"

class Jugador : public Personaje
{
public:
    Jugador(const QString &rutaQuieto, int framesQuieto, const QString &rutaAtaque, int framesAtaque, qreal posX, qreal posY, float vidaMaxima);

    bool bolaEnZonaBateo(Proyectil *bola) const;
    bool batear(Proyectil *bola, const QPointF &destino);
    void controlarProyectil(Proyectil *bola, int tecla);

    void setPuedeBatear(bool puede);
    bool getPuedeBatear() const;

private:
    Proyectil *bolaControlada;
    bool puedeBatear;
};

#endif
