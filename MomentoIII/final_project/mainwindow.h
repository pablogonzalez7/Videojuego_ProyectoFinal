#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QRectF>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QList>

#include "personaje.h"
#include "proyectil.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setEscena(short numEscena);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_botonInicio_clicked();

public slots:
    void moveFig();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    QGraphicsPixmapItem *fondo;
    QGraphicsTextItem *text;
    QGraphicsLineItem *l1;
    QGraphicsEllipseItem *e1;
    QTimer *timer, *cronometro, *timerFr;
    QString rutaFondoActual;
    QGraphicsItem *figEn, *fig;
    Personaje *vegito, *freezer;
    Proyectil *bolaFreezer;

    QList <QGraphicsRectItem*> obst; //Lista de apuntadores a items rectangulares

    void mostrarMenuInicio();
    void ponerFondo(QString, float opacity=0.8);

    void ajustarFondo();

    float x, y, ancho, alto;
};
#endif // MAINWINDOW_H
