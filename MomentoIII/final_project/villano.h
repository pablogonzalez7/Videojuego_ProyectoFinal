#ifndef VILLANO_H
#define VILLANO_H

#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QVector>

#include "personaje.h"
#include "proyectil.h"

/*
    Clase Villano

    Hereda de Personaje y contiene la lógica propia del enemigo.
    Los métodos percibir, razonar, actuar y aprender representan el agente
    inteligente sencillo pedido en el proyecto, sin crear una clase adicional.
*/
class Villano : public Personaje
{
public:
    struct Ataque {
        float velocidad;
        float dano;
        QString spriteProyectil;
        QString spriteLanzamiento;
        int framesLanzamiento;
        QVector<QRectF> framesRecorteLanzamiento;
    };

    Villano(const QString &rutaQuieto,int framesQuieto,const QString &rutaAtaque,int framesAtaque,qreal posX,qreal posY,float vidaMaxima);

    void configurarAtaques(const QList<Ataque> &nuevosAtaques);
    Ataque elegirAtaque();
    void reproducirAtaqueActual();

    void percibir(bool aciertoJugador);
    void percibirPosicionJugador(const QPointF &posicionJugador);
    short razonar();
    QPointF razonarDestinoLejano(const QRectF &limitesEscena) const;
    Proyectil *actuar(QGraphicsScene *scene, QPointF posicionInicial);
    void aprender(bool aciertoJugador);
    void aprenderTrayectoria(bool impactoJugador);

    float getVelocidadAtaque() const;
    short getTipoAtaqueActual() const;

private:
    QList<Ataque> ataques;
    QList<int> aciertosPorAtaque;
    QList<int> fallosPorAtaque;
    bool ultimoAciertoJugador;
    QPointF ultimaPosicionJugador;
    short tipoAtaqueActual;
    float velocidadAtaque;
    int sesgoCentro;
};

#endif // VILLANO_H
