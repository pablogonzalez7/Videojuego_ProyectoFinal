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

#include "sprite.h"


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

    sprite *mostrarSprite(const QString &rutaImagen, int cantidadFrames, qreal posX, qreal posY);
    void setEscena(short numEscena);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_botonInicio_clicked();
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
    QTimer *timer, *cronometro;
    QString rutaFondoActual;
    QGraphicsItem *figEn, *fig;

    QList <QGraphicsRectItem*> obst; //Lista de apuntadores a items rectangulares

    void mostrarMenuInicio();
    void ponerFondo(QString);

    void ajustarFondo();

    float x, y, ancho, alto;
};
#endif // MAINWINDOW_H
