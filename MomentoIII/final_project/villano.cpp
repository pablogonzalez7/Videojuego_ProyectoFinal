#include "villano.h"

#include <QRandomGenerator>

Villano::Villano(const QString &rutaQuieto,
                 int framesQuieto,
                 const QString &rutaAtaque,
                 int framesAtaque,
                 qreal posX,
                 qreal posY,
                 float vidaMaxima)
    : Personaje(rutaQuieto,
                framesQuieto,
                rutaAtaque,
                framesAtaque,
                posX,
                posY,
                vidaMaxima)
{
    ultimoAciertoJugador = false;
    tipoAtaqueActual = 0;
    velocidadAtaque = 3.6f;
}

void Villano::configurarAtaques(const QList<Ataque> &nuevosAtaques)
{
    ataques = nuevosAtaques;
    aciertosPorAtaque.clear();
    fallosPorAtaque.clear();

    for (int i = 0; i < ataques.size(); i++) {
        aciertosPorAtaque.append(0);
        fallosPorAtaque.append(0);
    }
}

Villano::Ataque Villano::elegirAtaque()
{
    if (ataques.isEmpty()) {
        tipoAtaqueActual = 0;
        velocidadAtaque = 3.6f;
        return {3.6f, 1.0f, ":/images/sprites/bolaFreezer.png"};
    }

    tipoAtaqueActual = razonar();

    if (tipoAtaqueActual < 0 || tipoAtaqueActual >= ataques.size()) {
        tipoAtaqueActual = 0;
    }

    velocidadAtaque = ataques.at(tipoAtaqueActual).velocidad;
    return ataques.at(tipoAtaqueActual);
}

void Villano::percibir(bool aciertoJugador)
{
    ultimoAciertoJugador = aciertoJugador;
}

short Villano::razonar()
{
    if (ataques.isEmpty()) {
        return 0;
    }

    /*
        Razonamiento sencillo:
        - Si solo hay un ataque, siempre se usa ese.
        - Si hay varios ataques, se favorece el ataque con el que el jugador
          ha fallado más. Si aún no hay historial, se escoge aleatoriamente.
    */
    int mejorIndice = QRandomGenerator::global()->bounded(ataques.size());
    int mayorFallos = -1;

    for (int i = 0; i < fallosPorAtaque.size(); i++) {
        if (fallosPorAtaque.at(i) > mayorFallos) {
            mayorFallos = fallosPorAtaque.at(i);
            mejorIndice = i;
        }
    }

    return static_cast<short>(mejorIndice);
}

Proyectil *Villano::actuar(QGraphicsScene *scene, QPointF posicionInicial)
{
    Ataque ataque = elegirAtaque();
    reproducirAtaque();

    Proyectil *bola = new Proyectil(scene, ataque.sprite, posicionInicial);
    bola->configurarAtaque(ataque.velocidad, ataque.dano);
    return bola;
}

void Villano::aprender(bool aciertoJugador)
{
    if (tipoAtaqueActual < 0 || tipoAtaqueActual >= ataques.size()) {
        return;
    }

    if (aciertoJugador) {
        aciertosPorAtaque[tipoAtaqueActual]++;
    }
    else {
        fallosPorAtaque[tipoAtaqueActual]++;
    }
}

float Villano::getVelocidadAtaque() const
{
    return velocidadAtaque;
}

short Villano::getTipoAtaqueActual() const
{
    return tipoAtaqueActual;
}
