#include "items.h"

#include <QtGlobal>

Items::Items()
{
    fondo = nullptr;
    relleno = nullptr;
    borde = nullptr;
    textoEscena = nullptr;
    elipseHitbox = nullptr;
    trayectoriaHitbox = nullptr;
    anchoBarra = 0.0;
    altoBarra = 0.0;
    posicionX = 0.0;
    posicionY = 0.0;
}

void Items::crearBarraVida(QGraphicsScene *scene, qreal x, qreal y, qreal ancho, qreal alto, const QString &nuevoTexto, const QColor &colorRelleno)
{
    destruir();

    if (scene == nullptr) {
        return;
    }

    posicionX = x;
    posicionY = y;
    anchoBarra = ancho;
    altoBarra = alto;
    etiqueta = nuevoTexto;

    fondo = scene->addRect(posicionX,
                           posicionY,
                           anchoBarra,
                           altoBarra,
                           QPen(Qt::black, 2),
                           QBrush(QColor(35, 35, 35, 190)));

    relleno = scene->addRect(posicionX + 2,
                             posicionY + 2,
                             anchoBarra - 4,
                             altoBarra - 4,
                             QPen(Qt::NoPen),
                             QBrush(colorRelleno));

    borde = scene->addRect(posicionX,
                           posicionY,
                           anchoBarra,
                           altoBarra,
                           QPen(Qt::white, 2),
                           QBrush(Qt::NoBrush));

    textoEscena = scene->addText(etiqueta);
    textoEscena->setDefaultTextColor(Qt::white);
    textoEscena->setFont(QFont("Arial", 12, QFont::Bold));
    textoEscena->setPos(posicionX, posicionY - 24);

    fondo->setZValue(20);
    relleno->setZValue(21);
    borde->setZValue(22);
    textoEscena->setZValue(23);
}

void Items::actualizarBarraVida(float vidaActual, float vidaMaxima)
{
    if (relleno == nullptr || textoEscena == nullptr) {
        return;
    }

    float vidaNormalizada = 0.0f;

    if (vidaMaxima > 0.0f) {
        vidaNormalizada = vidaActual / vidaMaxima;
    }

    vidaNormalizada = qBound(0.0f, vidaNormalizada, 1.0f);

    const qreal anchoRelleno = (anchoBarra - 4) * vidaNormalizada;
    relleno->setRect(posicionX + 2,
                     posicionY + 2,
                     anchoRelleno,
                     altoBarra - 4);

    textoEscena->setPlainText(etiqueta +
                              ": " +
                              QString::number(vidaActual, 'f', 1) +
                              " / " +
                              QString::number(vidaMaxima, 'f', 1));
}

void Items::crearTextoEscena(QGraphicsScene *scene, qreal x, qreal y, const QString &texto, const QColor &color, int tamañoFuente, bool negrita)
{
    destruir();

    if (scene == nullptr) {
        return;
    }

    textoEscena = scene->addText(texto);
    textoEscena->setDefaultTextColor(color);
    textoEscena->setFont(QFont("Arial",
                               tamañoFuente,
                               negrita ? QFont::Bold : QFont::Normal));
    textoEscena->setPos(x, y);
    textoEscena->setZValue(24);
}

void Items::setTexto(const QString &texto)
{
    if (textoEscena != nullptr) {
        textoEscena->setPlainText(texto);
    }
}

void Items::crearHitboxEliptica(QGraphicsScene *scene, const QRectF &recta, const QColor &color, qreal opacidad, bool visible)
{
    destruir();

    if (scene == nullptr) {
        return;
    }

    elipseHitbox = scene->addEllipse(recta,
                                     QPen(color, 2),
                                     QBrush(Qt::NoBrush));
    elipseHitbox->setOpacity(opacidad);
    elipseHitbox->setVisible(visible);
    elipseHitbox->setZValue(16);

    hitboxPath = QPainterPath();
    hitboxPath.addEllipse(recta);
}

void Items::crearHitboxTrayectoria(QGraphicsScene *scene, const QPainterPath &trayectoria, const QColor &color, qreal opacidad, bool visible)
{
    destruir();

    if (scene == nullptr) {
        return;
    }

    trayectoriaHitbox = scene->addPath(trayectoria,
                                       QPen(color, 2),
                                       QBrush(Qt::NoBrush));
    trayectoriaHitbox->setOpacity(opacidad);
    trayectoriaHitbox->setVisible(visible);
    trayectoriaHitbox->setZValue(16);
    hitboxPath = trayectoria;
}

bool Items::contienePunto(const QPointF &punto) const
{
    return !hitboxPath.isEmpty() && hitboxPath.contains(punto);
}

bool Items::colisionaCon(QGraphicsItem *item) const
{
    if (item == nullptr || hitboxPath.isEmpty()) {
        return false;
    }

    return hitboxPath.intersects(item->sceneBoundingRect());
}

void Items::setVisible(bool visible)
{
    if (fondo != nullptr) {
        fondo->setVisible(visible);
    }

    if (relleno != nullptr) {
        relleno->setVisible(visible);
    }

    if (borde != nullptr) {
        borde->setVisible(visible);
    }

    if (textoEscena != nullptr) {
        textoEscena->setVisible(visible);
    }

    if (elipseHitbox != nullptr) {
        elipseHitbox->setVisible(visible);
    }

    if (trayectoriaHitbox != nullptr) {
        trayectoriaHitbox->setVisible(visible);
    }
}

void Items::destruirItemsGraficos()
{
    if (fondo != nullptr) {
        if (fondo->scene() != nullptr) {
            fondo->scene()->removeItem(fondo);
        }

        delete fondo;
        fondo = nullptr;
    }

    if (relleno != nullptr) {
        if (relleno->scene() != nullptr) {
            relleno->scene()->removeItem(relleno);
        }

        delete relleno;
        relleno = nullptr;
    }

    if (borde != nullptr) {
        if (borde->scene() != nullptr) {
            borde->scene()->removeItem(borde);
        }

        delete borde;
        borde = nullptr;
    }

    if (textoEscena != nullptr) {
        if (textoEscena->scene() != nullptr) {
            textoEscena->scene()->removeItem(textoEscena);
        }

        delete textoEscena;
        textoEscena = nullptr;
    }

    if (elipseHitbox != nullptr) {
        if (elipseHitbox->scene() != nullptr) {
            elipseHitbox->scene()->removeItem(elipseHitbox);
        }

        delete elipseHitbox;
        elipseHitbox = nullptr;
    }

    if (trayectoriaHitbox != nullptr) {
        if (trayectoriaHitbox->scene() != nullptr) {
            trayectoriaHitbox->scene()->removeItem(trayectoriaHitbox);
        }

        delete trayectoriaHitbox;
        trayectoriaHitbox = nullptr;
    }
}

void Items::destruir()
{
    destruirItemsGraficos();
    hitboxPath = QPainterPath();
}

void Items::aplicarEstiloPanel(QLabel *label, const QString &colorFondo, const QString &colorBorde, const QString &colorTexto, int radio, int padding, int tamañoFuente, bool negrita)
{
    if (label == nullptr) {
        return;
    }

    const QString estilo = QString("QLabel {")
                           + "background-color: " + colorFondo + ";"
                           + "border: 2px solid " + colorBorde + ";"
                           + "border-radius: " + QString::number(radio) + "px;"
                           + "padding: " + QString::number(padding) + "px;"
                           + "color: " + colorTexto + ";"
                           + "font-size: " + QString::number(tamañoFuente) + "px;"
                           + "font-weight: " + QString(negrita ? "bold" : "normal") + ";"
                           + "}";
    label->setStyleSheet(estilo);
}
