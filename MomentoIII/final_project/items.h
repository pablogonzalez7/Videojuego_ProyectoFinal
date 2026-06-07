#ifndef ITEMS_H
#define ITEMS_H

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QLabel>
#include <QPainterPath>
#include <QPen>
#include <QString>

class Items
{
public:
    Items();

    void crearBarraVida(QGraphicsScene *scene,
                        qreal x,
                        qreal y,
                        qreal ancho,
                        qreal alto,
                        const QString &etiqueta,
                        const QColor &colorRelleno);
    void actualizarBarraVida(float vidaActual, float vidaMaxima);

    void crearTextoEscena(QGraphicsScene *scene,
                          qreal x,
                          qreal y,
                          const QString &texto,
                          const QColor &color = Qt::white,
                          int tamanoFuente = 12,
                          bool negrita = true);
    void setTexto(const QString &texto);

    void crearHitboxEliptica(QGraphicsScene *scene,
                             const QRectF &recta,
                             const QColor &color = QColor(255, 235, 90),
                             qreal opacidad = 0.0,
                             bool visible = false);
    void crearHitboxTrayectoria(QGraphicsScene *scene,
                                const QPainterPath &trayectoria,
                                const QColor &color = QColor(255, 255, 255),
                                qreal opacidad = 0.0,
                                bool visible = false);

    bool contienePunto(const QPointF &punto) const;
    bool colisionaCon(QGraphicsItem *item) const;

    void setVisible(bool visible);
    void destruir();

    static void aplicarEstiloPanel(QLabel *label,
                                   const QString &colorFondo,
                                   const QString &colorBorde,
                                   const QString &colorTexto,
                                   int radio,
                                   int padding,
                                   int tamanoFuente,
                                   bool negrita);

private:
    void destruirItemsGraficos();

    QGraphicsRectItem *fondo;
    QGraphicsRectItem *relleno;
    QGraphicsRectItem *borde;
    QGraphicsTextItem *textoEscena;
    QGraphicsEllipseItem *elipseHitbox;
    QGraphicsPathItem *trayectoriaHitbox;
    QPainterPath hitboxPath;

    qreal anchoBarra;
    qreal altoBarra;
    qreal posicionX;
    qreal posicionY;
    QString etiqueta;
};

#endif // ITEMS_H
