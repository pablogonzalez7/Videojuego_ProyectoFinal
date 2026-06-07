#include "villano.h"

#include <QLineF>
#include <QtMath>
#include <QRandomGenerator>

Villano::Villano(const QString &rutaQuieto, int framesQuieto, const QString &rutaAtaque, int framesAtaque, qreal posX, qreal posY, float vidaMaxima)
    : Personaje(rutaQuieto, framesQuieto, rutaAtaque, framesAtaque, posX, posY, vidaMaxima)
{
    ultimaPosicionJugador = QPointF(posX, posY);
    tipoAtaqueActual = 0;
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
        return {3.6f,
                1.0f,
                ":/images/sprites/bolaFreezer.png",
                ":/images/sprites/freezerPitcher.png",
                6,
                {}};
    }

    tipoAtaqueActual = razonar();

    if (tipoAtaqueActual < 0 || tipoAtaqueActual >= ataques.size()) {
        tipoAtaqueActual = 0;
    }

    return ataques.at(tipoAtaqueActual);
}

void Villano::reproducirAtaqueActual()
{
    if (tipoAtaqueActual < 0 || tipoAtaqueActual >= ataques.size()) {
        reproducirAtaque();
        return;
    }

    const Ataque &ataque = ataques.at(tipoAtaqueActual);
    if (!ataque.framesRecorteLanzamiento.isEmpty()) {
        tiempoAcumuladoMs = 0;
        tiempoRetencionAnimacionMs = 0;
        animacionActual = AnimacionAtaque;
        spritePersonaje->cambiarSprite(ataque.spriteLanzamiento, ataque.framesRecorteLanzamiento);
        spritePersonaje->reiniciarAnimacion();
        return;
    }

    iniciarAnimacion({ataque.spriteLanzamiento, ataque.framesLanzamiento}, AnimacionAtaque);
}

void Villano::percibirPosicionJugador(const QPointF &posicionJugador)
{
    ultimaPosicionJugador = posicionJugador;
}

short Villano::razonar()
{
    if (ataques.isEmpty()) {
        return 0;
    }

    if (ataques.size() == 1) {
        return 0;
    }

    int mejorIndice = QRandomGenerator::global()->bounded(ataques.size());
    int mayorFallos = fallosPorAtaque.at(mejorIndice);

    for (int i = 0; i < fallosPorAtaque.size(); i++) {
        if (fallosPorAtaque.at(i) > mayorFallos) {
            mayorFallos = fallosPorAtaque.at(i);
            mejorIndice = i;
        }
    }

    if (QRandomGenerator::global()->bounded(100) < 45) {
        mejorIndice = QRandomGenerator::global()->bounded(ataques.size());
    }

    return static_cast<short>(mejorIndice);
}

QPointF Villano::razonarDestinoLejano(const QRectF &limitesEscena) const
{


    const qreal limiteIzquierdo = limitesEscena.left() + 85;
    const qreal limiteDerecho = limitesEscena.right() - 135;
    const qreal limiteSuperior = limitesEscena.top() + 110;
    const qreal limiteInferior = limitesEscena.bottom() - 170;
    const QPointF posicionBlack(posicionX, posicionY);
    const qreal distanciaABlack = QLineF(posicionBlack, ultimaPosicionJugador).length();

    const qreal distanciaMinimaUtil = 120.0;
    const qreal distanciaMaximaUtil = 540.0;
    qreal distanciaNormalizada = (distanciaABlack - distanciaMinimaUtil) /
                                 (distanciaMaximaUtil - distanciaMinimaUtil);
    distanciaNormalizada = qBound(0.0, distanciaNormalizada, 1.0);


    const qreal distanciaObjetivoAJugador = 38.0 + (1.0 - distanciaNormalizada) * 290.0;

    QPointF mejorDestino = ultimaPosicionJugador;
    qreal mejorPuntaje = 1.0e9;

    for (int i = 0; i < 24; i++) {
        const qreal candidatoX = limiteIzquierdo +
                                 QRandomGenerator::global()->generateDouble() *
                                 (limiteDerecho - limiteIzquierdo);
        const qreal candidatoY = limiteSuperior +
                                 QRandomGenerator::global()->generateDouble() *
                                 (limiteInferior - limiteSuperior);
        const QPointF candidato(candidatoX, candidatoY);
        const qreal distanciaAJugador = QLineF(candidato, ultimaPosicionJugador).length();
        const qreal distanciaACentroDerecha = qAbs(candidatoX - (limiteIzquierdo + (limiteDerecho - limiteIzquierdo) * 0.62));

        qreal puntaje = qAbs(distanciaAJugador - distanciaObjetivoAJugador);

        puntaje += distanciaACentroDerecha * 0.08;

        if (puntaje < mejorPuntaje) {
            mejorPuntaje = puntaje;
            mejorDestino = candidato;
        }
    }

    return mejorDestino;
}

Proyectil *Villano::actuar(QGraphicsScene *scene, QPointF posicionInicial)
{
    Ataque ataque = elegirAtaque();
    reproducirAtaqueActual();

    Proyectil *bola = new Proyectil(scene, ataque.spriteProyectil, posicionInicial);
    bola->configurarAtaque(ataque.velocidad, ataque.daño);
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
