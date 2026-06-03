#ifndef BARRAVIDA_H
#define BARRAVIDA_H

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QString>

/*
    Clase BarraVida

    Dibuja una barra de vida sencilla con texto. Se creó para sacar
    esta responsabilidad del MainWindow y dejar el HUD más ordenado.
*/
class BarraVida
{
public:
    BarraVida();

    void crear(QGraphicsScene *scene,
               qreal x,
               qreal y,
               qreal ancho,
               qreal alto,
               const QString &etiqueta,
               const QColor &colorRelleno);

    void actualizar(float vidaActual, float vidaMaxima);
    void setVisible(bool visible);
    void destruir();

private:
    QGraphicsRectItem *fondo;
    QGraphicsRectItem *relleno;
    QGraphicsRectItem *borde;
    QGraphicsTextItem *texto;

    qreal anchoBarra;
    qreal altoBarra;
    qreal posicionX;
    qreal posicionY;
    QString etiqueta;
};

#endif // BARRAVIDA_H
