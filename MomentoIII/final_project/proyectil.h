#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QString>
#include <QGraphicsScene>
#include <QList>
#include <QTimer>
#include <QObject>
#include <QGraphicsPixmapItem>


class Proyectil: public QObject
{
public:
    Proyectil(QGraphicsScene *scene, const QString &rutaImagen);

    //void addProyectil(QGraphicsScene *scene);
    void iniciarTimerProyectil();
    void movProyectil();


private:
    float vx, vy;
    QGraphicsPixmapItem *proyectil;
    QTimer *timerP;
};

#endif // PROYECTIL_H
