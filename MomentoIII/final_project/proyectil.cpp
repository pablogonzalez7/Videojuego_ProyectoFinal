#include "proyectil.h"

Proyectil::Proyectil(QGraphicsScene *scene, const QString &rutaImagen)
{
    QPixmap pixmap(rutaImagen);
    proyectil = scene->addPixmap(pixmap);
    proyectil->setPos(650,270);
    vx = 0;
    vy = 5;

    timerP = new QTimer(this);
    connect(timerP, &QTimer::timeout, this, &Proyectil::movProyectil);

}


void Proyectil::movProyectil()
{
    proyectil->moveBy(vx,vy);
}

void Proyectil::iniciarTimerProyectil(){
    timerP->start(16);
}

