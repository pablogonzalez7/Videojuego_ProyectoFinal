#include "jugador.h"

#include <QKeyEvent>

Jugador::Jugador(const QString &rutaQuieto, int framesQuieto, const QString &rutaAtaque, int framesAtaque, qreal posX, qreal posY, float vidaMaxima)
    : Personaje(rutaQuieto, framesQuieto, rutaAtaque, framesAtaque, posX, posY, vidaMaxima)
{
    bolaControlada = nullptr;
    puedeBatear = true;
}

bool Jugador::bolaEnZonaBateo(Proyectil *bola) const
{
    if (bola == nullptr || spritePersonaje == nullptr || bola->getItem() == nullptr) {
        return false;
    }

    return spritePersonaje->collidesWithItem(bola->getItem(), Qt::IntersectsItemBoundingRect);
}

bool Jugador::batear(Proyectil *bola, const QPointF &destino)
{
    if (bola == nullptr || !puedeBatear) {
        return false;
    }

    if (bola->getEstado() != Proyectil::LanzadaPorFreezer) {
        return false;
    }

    if (!bolaEnZonaBateo(bola)) {
        return false;
    }

    reproducirAtaque();
    bola->iniciarBateo(destino.x(), destino.y());
    bolaControlada = bola;
    return true;
}

void Jugador::controlarProyectil(Proyectil *bola, int tecla)
{
    if (bola == nullptr || bola->getEstado() != Proyectil::BateadaPorVegito) {
        return;
    }

    if (tecla == Qt::Key_A) {
        bola->ajustarDestino(-25, 0);
    }
    else if (tecla == Qt::Key_D) {
        bola->ajustarDestino(25, 0);
    }
    else if (tecla == Qt::Key_W) {
        bola->ajustarDestino(0, -25);
    }
    else if (tecla == Qt::Key_S) {
        bola->ajustarDestino(0, 25);
    }
}

void Jugador::setPuedeBatear(bool puede)
{
    puedeBatear = puede;
}

bool Jugador::getPuedeBatear() const
{
    return puedeBatear;
}
