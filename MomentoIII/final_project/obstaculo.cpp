#include "obstaculo.h"

#include <QtMath>

Obstaculo::Obstaculo(QGraphicsScene *scene,const QString &rutaImagen,QPointF posicion,TipoMovimiento tipoMovimiento)
{
    posicionInicial = posicion;
    tipo = rutaImagen;
    amplitud = 35.0f;
    frecuencia = 2.0f;
    movimiento = tipoMovimiento;
    item = nullptr;

    if (scene != nullptr) {
        QPixmap pixmap(rutaImagen);
        pixmap = pixmap.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        item = scene->addPixmap(pixmap);
        item->setOffset(-pixmap.width() / 2.0, -pixmap.height() / 2.0);
        item->setPos(posicionInicial);
        item->setZValue(4);
    }
}

Obstaculo::~Obstaculo()
{
    if (item != nullptr) {
        if (item->scene() != nullptr) {
            item->scene()->removeItem(item);
        }

        delete item;
        item = nullptr;
    }
}

void Obstaculo::actualizar(float tiempo)
{
    if (movimiento == Orbital) {
        moverOrbital(tiempo);
    }
    else if (movimiento == Amortiguado) {
        moverAmortiguado(tiempo);
    }
}

void Obstaculo::moverOrbital(float tiempo)
{
    if (item == nullptr) {
        return;
    }

    qreal x = posicionInicial.x() + amplitud * qCos(frecuencia * tiempo);
    qreal y = posicionInicial.y() + amplitud * qSin(frecuencia * tiempo);
    item->setPos(x, y);
}

void Obstaculo::moverAmortiguado(float tiempo)
{
    if (item == nullptr) {
        return;
    }

    qreal desplazamiento = amplitud * qExp(-0.8 * tiempo) * qSin(frecuencia * 6 * tiempo);
    item->setPos(posicionInicial.x() + desplazamiento, posicionInicial.y());
}

bool Obstaculo::colisionaCon(Proyectil *proyectil) const
{
    if (item == nullptr || proyectil == nullptr || proyectil->getItem() == nullptr) {
        return false;
    }

    return item->collidesWithItem(proyectil->getItem(), Qt::IntersectsItemBoundingRect);
}

void Obstaculo::aplicarEfecto(Proyectil *proyectil)
{
    if (proyectil == nullptr) {
        return;
    }

    // Efecto sencillo preparado para nivel 2: empuja el destino del proyectil.
    proyectil->ajustarDestino(25, -15);
}

void Obstaculo::setVisible(bool visible)
{
    if (item != nullptr) {
        item->setVisible(visible);
    }
}
