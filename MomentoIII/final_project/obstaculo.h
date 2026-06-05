#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QString>

#include "proyectil.h"

/*
    Clase Obstaculo

    Clase general para objetos del escenario, especialmente pensada para el
    nivel 2: meteoritos, anillos de poder o zonas que alteran trayectorias.
    No afecta el nivel 1 actual si no se crean obstáculos.
*/
class Obstaculo
{
public:
    enum TipoMovimiento {
        Fijo,
        Orbital,
        Amortiguado
    };

    Obstaculo(QGraphicsScene *scene,
              const QString &rutaImagen,
              QPointF posicionInicial,
              TipoMovimiento tipoMovimiento = Fijo);

    ~Obstaculo();

    void actualizar(float tiempo);
    void moverOrbital(float tiempo);
    void moverAmortiguado(float tiempo);
    bool colisionaCon(Proyectil *proyectil) const;
    void aplicarEfecto(Proyectil *proyectil);
    void setVisible(bool visible);

private:
    QGraphicsPixmapItem *item;
    QPointF posicionInicial;
    QString tipo;
    float amplitud;
    float frecuencia;
    TipoMovimiento movimiento;
};

#endif // OBSTACULO_H
#ifndef OBSTACULO_H
#define OBSTACULO_H

class Obstaculo
{
public:
    Obstaculo();
};

#endif // OBSTACULO_H
