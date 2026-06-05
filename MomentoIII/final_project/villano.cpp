#include "villano.h"

#include <QRandomGenerator>

Villano::Villano(const QString &rutaQuieto,int framesQuieto,const QString &rutaAtaque,int framesAtaque,qreal posX,qreal posY,float vidaMaxima)
    : Personaje(rutaQuieto,framesQuieto,rutaAtaque,framesAtaque,posX,posY,vidaMaxima)
{
    ultimoAciertoJugador = false;
    ultimaPosicionJugador = QPointF(posX, posY);
    tipoAtaqueActual = 0;
    velocidadAtaque = 3.6f;
    sesgoCentro = 1;
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

    velocidadAtaque = ataques.at(tipoAtaqueActual).velocidad;
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

void Villano::percibir(bool aciertoJugador)
{
    ultimoAciertoJugador = aciertoJugador;
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

    // Mantiene variedad visible en dificil: aun si hay un ataque preferido
    // por el historial, Freezer explora otros lanzamientos con frecuencia.
    if (QRandomGenerator::global()->bounded(100) < 45) {
        mejorIndice = QRandomGenerator::global()->bounded(ataques.size());
    }

    return static_cast<short>(mejorIndice);
}

QPointF Villano::razonarDestinoLejano(const QRectF &limitesEscena) const
{
    const qreal margenHorizontal = limitesEscena.width() * 0.12;
    const qreal margenVertical = limitesEscena.height() * 0.18;
    const qreal centroEscena = limitesEscena.center().x();
    const qreal umbralCentro = limitesEscena.width() * 0.10;

    qreal destinoX = limitesEscena.left() + margenHorizontal;

    if (ultimaPosicionJugador.x() < centroEscena - umbralCentro) {
        destinoX = limitesEscena.right() - margenHorizontal;
    }
    else if (ultimaPosicionJugador.x() <= centroEscena + umbralCentro && sesgoCentro > 0) {
        destinoX = limitesEscena.right() - margenHorizontal;
    }

    return QPointF(destinoX, limitesEscena.bottom() - margenVertical);
}

Proyectil *Villano::actuar(QGraphicsScene *scene, QPointF posicionInicial)
{
    Ataque ataque = elegirAtaque();
    reproducirAtaqueActual();

    Proyectil *bola = new Proyectil(scene, ataque.spriteProyectil, posicionInicial);
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

void Villano::aprenderTrayectoria(bool impactoJugador)
{
    if (impactoJugador) {
        sesgoCentro = -sesgoCentro;
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
