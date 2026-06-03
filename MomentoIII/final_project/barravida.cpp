#include "barravida.h"

BarraVida::BarraVida()
{
    fondo = nullptr;
    relleno = nullptr;
    borde = nullptr;
    texto = nullptr;

    anchoBarra = 0;
    altoBarra = 0;
    posicionX = 0;
    posicionY = 0;
}

void BarraVida::crear(QGraphicsScene *scene,
                      qreal x,
                      qreal y,
                      qreal ancho,
                      qreal alto,
                      const QString &nuevoTexto,
                      const QColor &colorRelleno)
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

    texto = scene->addText(etiqueta);
    texto->setDefaultTextColor(Qt::white);
    texto->setFont(QFont("Arial", 12, QFont::Bold));
    texto->setPos(posicionX, posicionY - 24);

    fondo->setZValue(20);
    relleno->setZValue(21);
    borde->setZValue(22);
    texto->setZValue(23);
}

void BarraVida::actualizar(float vidaActual, float vidaMaxima)
{
    if (relleno == nullptr || texto == nullptr) {
        return;
    }

    float vidaNormalizada = 0.0f;

    if (vidaMaxima > 0.0f) {
        vidaNormalizada = vidaActual / vidaMaxima;
    }

    if (vidaNormalizada < 0.0f) {
        vidaNormalizada = 0.0f;
    }

    if (vidaNormalizada > 1.0f) {
        vidaNormalizada = 1.0f;
    }

    qreal anchoRelleno = (anchoBarra - 4) * vidaNormalizada;
    relleno->setRect(posicionX + 2,
                     posicionY + 2,
                     anchoRelleno,
                     altoBarra - 4);

    texto->setPlainText(etiqueta +
                        ": " +
                        QString::number(vidaActual, 'f', 1) +
                        " / " +
                        QString::number(vidaMaxima, 'f', 1));
}

void BarraVida::setVisible(bool visible)
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

    if (texto != nullptr) {
        texto->setVisible(visible);
    }
}

void BarraVida::destruir()
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

    if (texto != nullptr) {
        if (texto->scene() != nullptr) {
            texto->scene()->removeItem(texto);
        }

        delete texto;
        texto = nullptr;
    }
}
