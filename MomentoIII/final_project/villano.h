#ifndef VILLANO_H
#define VILLANO_H

#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QVector>

#include "personaje.h"
#include "proyectil.h"

class Villano : public Personaje
{
public:
    struct Ataque {
        float velocidad;
        float daño;
        QString spriteProyectil;
        QString spriteLanzamiento;
        int framesLanzamiento;
        QVector<QRectF> framesRecorteLanzamiento;
    };

    Villano(const QString &rutaQuieto, int framesQuieto, const QString &rutaAtaque, int framesAtaque, qreal posX, qreal posY, float vidaMaxima);

    void configurarAtaques(const QList<Ataque> &nuevosAtaques);
    Ataque elegirAtaque();
    void reproducirAtaqueActual();
    void percibirPosicionJugador(const QPointF &posicionJugador);
    short razonar();
    QPointF razonarDestinoLejano(const QRectF &limitesEscena) const;
    Proyectil *actuar(QGraphicsScene *scene, QPointF posicionInicial);
    void aprender(bool aciertoJugador);

private:
    QList<Ataque> ataques;
    QList<int> aciertosPorAtaque;
    QList<int> fallosPorAtaque;
    QPointF ultimaPosicionJugador;
    short tipoAtaqueActual;
};

#endif
